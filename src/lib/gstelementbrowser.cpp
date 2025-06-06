#include "gstelementbrowser.h"

namespace GstStudio {

GstStudio::GstElementBrowser::GstElementBrowser(QObject* parent)
    : QObject(parent), m_parser(new GstInspectParser(this)), m_propertyModel(new GstPropertyModel(this)),
      m_padModel(new GstPadModel(this)) {
    connect(m_parser, &GstInspectParser::parsingFinished, this, &GstElementBrowser::onParsingFinished);
    connect(m_parser, &GstInspectParser::elementParsed, this, &GstElementBrowser::onElementParsed);
}

void GstStudio::GstElementBrowser::setSelectedElement(const QString& elementName) {
    if (m_selectedElement != elementName) {
        m_selectedElement = elementName;
        updateElementDetails();
        emit selectedElementChanged();
    }
}

void GstStudio::GstElementBrowser::refreshElements() {
    m_isLoading = true;
    emit loadingChanged();
    m_parser->parseAllElements();
}

void GstStudio::GstElementBrowser::filterElements(const QString& filter) {
    if (filter.isEmpty()) {
        m_filteredElementNames = m_elementNames;
    } else {
        m_filteredElementNames.clear();
        for (const QString& name : std::as_const(m_elementNames)) {
            if (name.contains(filter, Qt::CaseInsensitive)) {
                m_filteredElementNames.append(name);
            }
        }
    }
    emit elementNamesChanged();
}

void GstStudio::GstElementBrowser::onParsingFinished() {
    m_elementNames = m_parser->getAllElementNames();
    m_filteredElementNames = m_elementNames;
    m_isLoading = false;
    emit elementNamesChanged();
    emit loadingChanged();
}

void GstStudio::GstElementBrowser::onElementParsed(const QString& elementName) {
    // Could emit progress updates here if needed
}

void GstStudio::GstElementBrowser::updateElementDetails() {
    if (m_selectedElement.isEmpty()) {
        m_currentElement = GstElement();
    } else {
        m_currentElement = m_parser->getElement(m_selectedElement);
    }

    m_propertyModel->setProperties(m_currentElement.properties);
    m_padModel->setPadTemplates(m_currentElement.padTemplates);
    emit elementDetailsChanged();
}

} // namespace GstStudio

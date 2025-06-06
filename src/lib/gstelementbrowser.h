#pragma once

#include "gstinspectparser.h" // Your parser from previous artifact
#include "gstpadmodel.h"
#include "gstpropertymodel.h"
#include <QAbstractListModel>
#include <QObject>
#include <QQmlEngine>

namespace GstStudio {

class GstElementBrowser : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QStringList elementNames READ elementNames NOTIFY elementNamesChanged)
    Q_PROPERTY(QString selectedElement READ selectedElement WRITE setSelectedElement NOTIFY selectedElementChanged)
    Q_PROPERTY(QString elementDescription READ elementDescription NOTIFY elementDetailsChanged)
    Q_PROPERTY(QString elementClassification READ elementClassification NOTIFY elementDetailsChanged)
    Q_PROPERTY(QString elementAuthor READ elementAuthor NOTIFY elementDetailsChanged)
    Q_PROPERTY(GstPropertyModel* propertyModel READ propertyModel CONSTANT)
    Q_PROPERTY(GstPadModel* padModel READ padModel CONSTANT)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY loadingChanged)

  public:
    explicit GstElementBrowser(QObject* parent = nullptr);

    QStringList elementNames() const {
        return m_elementNames;
    }
    QString selectedElement() const {
        return m_selectedElement;
    }
    QString elementDescription() const {
        return m_currentElement.description;
    }
    QString elementClassification() const {
        return m_currentElement.classification;
    }
    QString elementAuthor() const {
        return m_currentElement.author;
    }
    GstPropertyModel* propertyModel() {
        return m_propertyModel;
    }
    GstPadModel* padModel() {
        return m_padModel;
    }
    bool isLoading() const {
        return m_isLoading;
    }

    void setSelectedElement(const QString& elementName);

  public slots:
    Q_INVOKABLE void refreshElements();
    Q_INVOKABLE void filterElements(const QString& filter);

  signals:
    void elementNamesChanged();
    void selectedElementChanged();
    void elementDetailsChanged();
    void loadingChanged();

  private slots:
    void onParsingFinished();
    void onElementParsed(const QString& elementName);

  private:
    GstInspectParser* m_parser;
    GstPropertyModel* m_propertyModel;
    GstPadModel* m_padModel;
    QStringList m_elementNames;
    QStringList m_filteredElementNames;
    QString m_selectedElement;
    GstElement m_currentElement;
    bool m_isLoading = false;

    void updateElementDetails();
};

} // namespace GstStudio

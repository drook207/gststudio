/**
 * @file gstelementbrowser.h
 * @brief QML-accessible browser for GStreamer elements
 * @author GstStudio Team
 */

#pragma once

#include "gstinspectparser.h" // Your parser from previous artifact
#include "gstpadmodel.h"
#include "gstpropertymodel.h"
#include <QAbstractListModel>
#include <QObject>
#include <QQmlEngine>

namespace GstStudio {

/**
 * @class GstElementBrowser
 * @brief QML-accessible browser for GStreamer elements
 *
 * This class provides a QML interface for browsing GStreamer elements.
 * It manages element discovery, selection, and provides models for
 * displaying element properties and pad templates.
 */
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
    /**
     * @brief Constructs a new GstElementBrowser
     * @param parent Parent QObject
     */
    explicit GstElementBrowser(QObject* parent = nullptr);

    /**
     * @brief Get list of available element names
     * @return QStringList of element names
     */
    [[nodiscard]] QStringList elementNames() const {
        return m_elementNames;
    }

    /**
     * @brief Get currently selected element name
     * @return Selected element name
     */
    [[nodiscard]] QString selectedElement() const {
        return m_selectedElement;
    }

    /**
     * @brief Get description of current element
     * @return Element description
     */
    [[nodiscard]] QString elementDescription() const {
        return m_currentElement.m_description;
    }

    /**
     * @brief Get classification of current element
     * @return Element classification
     */
    [[nodiscard]] QString elementClassification() const {
        return m_currentElement.m_classification;
    }

    /**
     * @brief Get author information of current element
     * @return Element author
     */
    [[nodiscard]] QString elementAuthor() const {
        return m_currentElement.m_author;
    }

    /**
     * @brief Get property model for current element
     * @return Pointer to GstPropertyModel
     */
    GstPropertyModel* propertyModel() {
        return m_propertyModel;
    }

    /**
     * @brief Get pad model for current element
     * @return Pointer to GstPadModel
     */
    GstPadModel* padModel() {
        return m_padModel;
    }

    /**
     * @brief Check if element parsing is in progress
     * @return true if loading, false otherwise
     */
    [[nodiscard]] bool isLoading() const {
        return m_isLoading;
    }

    /**
     * @brief Set the selected element
     * @param elementName Name of element to select
     */
    void setSelectedElement(const QString& elementName);

    /**
     * @brief Refresh the list of available elements
     */
    Q_INVOKABLE void refreshElements();

    /**
     * @brief Filter elements by name or classification
     * @param filter Filter string
     */
    Q_INVOKABLE void filterElements(const QString& filter);

  signals:
    /**
     * @brief Emitted when element names list changes
     */
    void elementNamesChanged();

    /**
     * @brief Emitted when selected element changes
     */
    void selectedElementChanged();

    /**
     * @brief Emitted when element details change
     */
    void elementDetailsChanged();

    /**
     * @brief Emitted when loading state changes
     */
    void loadingChanged();

  private slots:
    /**
     * @brief Called when element parsing is finished
     */
    void onParsingFinished();

    /**
     * @brief Called when a single element is parsed
     * @param elementName Name of the parsed element
     */
    void onElementParsed(const QString& elementName);

  private:
    GstInspectParser* m_parser;         ///< Parser for GStreamer elements
    GstPropertyModel* m_propertyModel;  ///< Model for element properties
    GstPadModel* m_padModel;            ///< Model for element pad templates
    QStringList m_elementNames;         ///< List of all element names
    QStringList m_filteredElementNames; ///< Filtered element names
    QString m_selectedElement;          ///< Currently selected element
    GstElement m_currentElement;        ///< Current element details
    bool m_isLoading = false;           ///< Loading state

    /**
     * @brief Update element details for current selection
     */
    void updateElementDetails();
};

} // namespace GstStudio

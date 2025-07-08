/**
 * @file gstpropertymodel.h
 * @brief Qt model for displaying GStreamer element properties
 * @author GstStudio Team
 */

#pragma once

#include "gstelement.h"
#include <QAbstractListModel>
#include <QQmlEngine>

namespace GstStudio {

/**
 * @class GstPropertyModel
 * @brief Qt model for displaying GStreamer element properties
 *
 * This model provides a QML-accessible interface for displaying
 * GStreamer element properties in list views.
 */
class GstPropertyModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT

  public:
    /**
     * @enum PropertyRoles
     * @brief Roles for accessing property data
     */
    enum PropertyRoles {
        NameRole = Qt::UserRole + 1, ///< Property name
        TypeRole,                    ///< Property type
        DescriptionRole,             ///< Property description
        DefaultValueRole,            ///< Default value
        RangeRole,                   ///< Valid range
        EnumValuesRole,              ///< Enum values
        WritableRole,                ///< Writable flag
        ReadableRole                 ///< Readable flag
    };

    /**
     * @brief Constructs a new GstPropertyModel
     * @param parent Parent QObject
     */
    explicit GstPropertyModel(QObject* parent = nullptr);

    /**
     * @brief Get number of properties
     * @param parent Parent model index (unused)
     * @return Number of properties
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief Get data for a specific property
     * @param index Model index
     * @param role Data role
     * @return QVariant containing requested data
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    /**
     * @brief Get role names for QML access
     * @return Hash of role names
     */
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Set properties to display
     * @param properties List of properties
     */
    void setProperties(const QList<GstProperty>& properties);

  private:
    QList<GstProperty> m_properties; ///< List of properties
};

} // namespace GstStudio

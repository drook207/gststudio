/**
 * @file gstpadmodel.h
 * @brief Qt model for displaying GStreamer pad templates
 * @author GstStudio Team
 */

#pragma once

#include "gstelement.h"
#include <QAbstractListModel>
#include <QQmlEngine>

namespace GstStudio {

/**
 * @class GstPadModel
 * @brief Qt model for displaying GStreamer pad templates
 *
 * This model provides a QML-accessible interface for displaying
 * GStreamer element pad templates in list views.
 */
class GstPadModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT

  public:
    /**
     * @enum PadRoles
     * @brief Roles for accessing pad template data
     */
    enum PadRoles {
        NAME_ROLE = Qt::UserRole + 1, ///< Pad template name
        DIRECTION_ROLE,               ///< Pad direction (SRC/SINK)
        PRESENCE_ROLE,                ///< Pad presence (ALWAYS/SOMETIMES/REQUEST)
        CAPS_ROLE                     ///< Pad capabilities
    };

    /**
     * @brief Constructs a new GstPadModel
     * @param parent Parent QObject
     */
    explicit GstPadModel(QObject* parent = nullptr);

    /**
     * @brief Get number of pad templates
     * @param parent Parent model index (unused)
     * @return Number of pad templates
     */
    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief Get data for a specific pad template
     * @param index Model index
     * @param role Data role
     * @return QVariant containing requested data
     */
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    /**
     * @brief Get role names for QML access
     * @return Hash of role names
     */
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Set pad templates to display
     * @param pads List of pad templates
     */
    void setPadTemplates(const QList<GstPadTemplate>& pads);

  private:
    QList<GstPadTemplate> m_pads; ///< List of pad templates
};

} // namespace GstStudio

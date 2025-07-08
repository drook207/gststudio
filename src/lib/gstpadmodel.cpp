#include "gstpadmodel.h"

namespace GstStudio {

GstStudio::GstPadModel::GstPadModel(QObject* parent) : QAbstractListModel(parent) {
}

int GstStudio::GstPadModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent)
    return static_cast<int>(m_pads.size());
}

QVariant GstStudio::GstPadModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_pads.size())
        return {};

    const GstPadTemplate& pad = m_pads.at(index.row());

    switch (role) {
        case NAME_ROLE:
            return pad.m_name;
        case DIRECTION_ROLE:
            return pad.m_direction;
        case PRESENCE_ROLE:
            return pad.m_presence;
        case CAPS_ROLE:
            return pad.m_caps;
        default:
            return {};
    }
}

QHash<int, QByteArray> GstStudio::GstPadModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NAME_ROLE] = "name";
    roles[DIRECTION_ROLE] = "direction";
    roles[PRESENCE_ROLE] = "presence";
    roles[CAPS_ROLE] = "caps";
    return roles;
}

void GstStudio::GstPadModel::setPadTemplates(const QList<GstPadTemplate>& pads) {
    beginResetModel();
    m_pads = pads;
    endResetModel();
}

} // namespace GstStudio

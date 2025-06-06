#include "gstpadmodel.h"

namespace GstStudio {

GstStudio::GstPadModel::GstPadModel(QObject* parent) : QAbstractListModel(parent) {
}

int GstStudio::GstPadModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent)
    return m_pads.size();
}

QVariant GstStudio::GstPadModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_pads.size())
        return QVariant();

    const GstPadTemplate& pad = m_pads.at(index.row());

    switch (role) {
        case NameRole:
            return pad.name;
        case DirectionRole:
            return pad.direction;
        case PresenceRole:
            return pad.presence;
        case CapsRole:
            return pad.caps;
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> GstStudio::GstPadModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[DirectionRole] = "direction";
    roles[PresenceRole] = "presence";
    roles[CapsRole] = "caps";
    return roles;
}

void GstStudio::GstPadModel::setPadTemplates(const QList<GstPadTemplate>& pads) {
    beginResetModel();
    m_pads = pads;
    endResetModel();
}

} // namespace GstStudio

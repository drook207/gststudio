#include "gstpropertymodel.h"

namespace GstStudio {

GstStudio::GstPropertyModel::GstPropertyModel(QObject* parent) : QAbstractListModel(parent) {
}

int GstStudio::GstPropertyModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent)
    return m_properties.size();
}

QVariant GstStudio::GstPropertyModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_properties.size())
        return QVariant();

    const GstProperty& prop = m_properties.at(index.row());

    switch (role) {
        case NameRole:
            return prop.name;
        case TypeRole:
            return prop.type;
        case DescriptionRole:
            return prop.description;
        case DefaultValueRole:
            return prop.defaultValue;
        case RangeRole:
            return prop.range;
        case EnumValuesRole:
            return QVariant::fromValue(prop.enumValues);
        case WritableRole:
            return prop.writable;
        case ReadableRole:
            return prop.readable;
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> GstStudio::GstPropertyModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[TypeRole] = "type";
    roles[DescriptionRole] = "description";
    roles[DefaultValueRole] = "defaultValue";
    roles[RangeRole] = "range";
    roles[EnumValuesRole] = "enumValues";
    roles[WritableRole] = "writable";
    roles[ReadableRole] = "readable";
    return roles;
}

void GstStudio::GstPropertyModel::setProperties(const QList<GstProperty>& properties) {
    beginResetModel();
    m_properties = properties;
    endResetModel();
}

} // namespace GstStudio

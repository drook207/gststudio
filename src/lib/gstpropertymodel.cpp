#include "gstpropertymodel.h"

namespace GstStudio {

GstStudio::GstPropertyModel::GstPropertyModel(QObject* parent) : QAbstractListModel(parent) {
}

int GstStudio::GstPropertyModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent)
    return static_cast<int>(m_properties.size());
}

QVariant GstStudio::GstPropertyModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_properties.size())
        return {};

    const GstProperty& prop = m_properties.at(index.row());

    switch (role) {
        case NameRole:
            return prop.m_name;
        case TypeRole:
            return prop.m_type;
        case DescriptionRole:
            return prop.m_description;
        case DefaultValueRole:
            return prop.m_defaultValue;
        case RangeRole:
            return prop.m_range;
        case EnumValuesRole:
            return QVariant::fromValue(prop.m_enumValues);
        case WritableRole:
            return prop.m_writable;
        case ReadableRole:
            return prop.m_readable;
        default:
            return {};
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

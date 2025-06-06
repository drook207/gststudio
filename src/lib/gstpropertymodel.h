#pragma once

#include "gstelement.h"
#include <QAbstractListModel>
#include <QQmlEngine>

namespace GstStudio {

class GstPropertyModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT

  public:
    enum PropertyRoles {
        NameRole = Qt::UserRole + 1,
        TypeRole,
        DescriptionRole,
        DefaultValueRole,
        RangeRole,
        EnumValuesRole,
        WritableRole,
        ReadableRole
    };

    explicit GstPropertyModel(QObject* parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setProperties(const QList<GstProperty>& properties);

  private:
    QList<GstProperty> m_properties;
};

} // namespace GstStudio

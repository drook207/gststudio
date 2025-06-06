#pragma once

#include "gstelement.h"
#include <QAbstractListModel>
#include <QQmlEngine>

namespace GstStudio {

class GstPadModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT

  public:
    enum PadRoles { NameRole = Qt::UserRole + 1, DirectionRole, PresenceRole, CapsRole };

    explicit GstPadModel(QObject* parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setPadTemplates(const QList<GstPadTemplate>& pads);

  private:
    QList<GstPadTemplate> m_pads;
};

} // namespace GstStudio

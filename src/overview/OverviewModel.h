#pragma once

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QDateTime>
#include <QPixmap>
#include <QStringList>
#include <QJsonArray>
#include <QDir>

#include "lib/utils.h"
#include "lib/config.h"
#include "lib/QRichItemDelegate.h"
#include "models/KodiSrc.h"

class OverviewModel : public QAbstractListModel
{
Q_OBJECT

public:
  enum OverviewModelRoles {
    statusIcon = 0,
    ContentRole,
    settingsIcon,
    COUNT
  };

  explicit OverviewModel(QObject *parent = nullptr);
  ~OverviewModel() override {
    this->clear();
  }

  QList<QSharedPointer<KodiSrc>> sources;

  void load();
  void clear();
  void saveToCfg();
  void deleteItem(const QSharedPointer<KodiSrc> ptr);
  void addItem(const QSharedPointer<KodiSrc> &ptr);
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

  uint32_t itemHeight = 80;

public slots:
  void onOverviewRowClicked(unsigned int idx);
  void onNetworkScanResult(const QString &scheme, const QString &address, unsigned int port);
  void itemUpdated(const QSharedPointer<KodiSrc> ptr);
  void itemUpdated(const QString &uuidv4);

private slots:
  void onTest();
  void onTestStatus(KodiStatus status);

signals:
  void overviewRowClicked(QSharedPointer<KodiSrc> ptr);

protected:
  QHash<int, QByteArray> roleNames() const;

private:
  void preloadPixmaps();
  QMap<QString, QPixmap> m_pixmaps;
};

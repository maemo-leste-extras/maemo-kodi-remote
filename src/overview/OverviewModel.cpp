#include <QObject>
#include <QDebug>

#include "overview/OverviewModel.h"

OverviewModel::OverviewModel(QObject *parent) : 
    QAbstractListModel(parent) {
  this->preloadPixmaps();
  this->load();

  // connect(this, &OverviewModel::dataChanged, [this](QModelIndex a, QModelIndex b){
  //   this->saveToCfg();
  // });
}

int OverviewModel::rowCount(const QModelIndex & parent) const {
  Q_UNUSED(parent);
  return sources.count();
}

int OverviewModel::columnCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return OverviewModelRoles::COUNT;
}

QVariant OverviewModel::data(const QModelIndex &index, int role) const {
  const auto row = index.row();
  if (row < 0 || row >= sources.count())
    return QVariant();
  const auto mdl = sources[row];

  if(role == Qt::DisplayRole) {
    switch(index.column()) {
      case OverviewModel::ContentRole: {
        mdl->generateOverviewItemDelegateRichText();
        return mdl->overviewItemDelegateRichText;
      }
      default:
        return QVariant();
    }
  } else if(role == Qt::DecorationRole) {
    switch (index.column()) {
      case OverviewModel::statusIcon: {
        QString icon;
        if(mdl->status >= KodiStatus::CHILLING) {
          icon = ":/assets/monitor_on";
        } else {
          icon = ":/assets/monitor_off";
        }
        
        if(m_pixmaps.contains(icon)) {
          return m_pixmaps[icon];
        }
      }
      case OverviewModel::settingsIcon: {
        const auto icon_default = ":/assets/settings";
        const auto icon_conference = "general_conference_avatar";

        if(m_pixmaps.contains(icon_default)) {
          return m_pixmaps[icon_default];
        }
      }
      default: {
        return QVariant();
      }
    }
  } else if(role == Qt::SizeHintRole) {
    switch (index.column()) {
      case OverviewModel::statusIcon: {
        return QSize(58, 54);
      }
      case OverviewModel::settingsIcon: {
        return QSize(92, 54);
      }
      default: {
        return QVariant();
      }
    }
  }

  return {};
}

void OverviewModel::onNetworkScanResult(const QString &scheme, const QString &address, unsigned int port) {
  for(const auto &kodisrc: sources) {
    if(kodisrc->address == address && kodisrc->port == port)
      return;  // already added
  }

  auto ptr = QSharedPointer<KodiSrc>(new KodiSrc("", address, scheme, port, "", ""));
  this->addItem(ptr);
}

void OverviewModel::load() {
  this->clear();
  QList<QSharedPointer<KodiSrc>> results;

  auto blob = config()->get(ConfigKeys::KodiSources).toByteArray();
  if(blob.isEmpty()) return;

  auto doc = QJsonDocument::fromJson(blob);
  if(doc.isNull() || !doc.isArray()) {
      qWarning() << "invalid json encountered parsing Config::KodiSources";
      return;
  }

  auto arr = doc.array();
  for (const auto &item: arr) {
    auto obj = item.toObject();
    auto name = obj["name"].toString();
    auto address = obj["address"].toString();
    auto port = obj["port"].toString().toInt();
    auto username = obj["username"].toString();
    auto password = obj["password"].toString();
    auto scheme = obj["scheme"].toString();

    QSharedPointer<KodiSrc> ptr(new KodiSrc(name, address, scheme, port, username, password));
    this->addItem(ptr);
  }

  beginInsertRows(QModelIndex(), 0, results.size());
  for (const auto &kodisrc: results)
    sources << kodisrc;
  endInsertRows();
}

void OverviewModel::onTest() {
  qDebug() << "onTest";
}

void OverviewModel::onTestStatus(KodiStatus status) {
  qDebug() << "onTest2";
}

void OverviewModel::deleteItem(const QSharedPointer<KodiSrc> ptr) {
  auto idx = sources.indexOf(ptr);
  if(idx == -1) return;

  beginRemoveRows(QModelIndex(), idx, idx + 1);
  sources.removeAll(ptr);
  endRemoveRows();

  //ptr->deleteLater();
  qDebug() << "sources lnegth" << sources.length();
}

void OverviewModel::itemUpdated(const QString &uuidv4) {
  for(const auto &ptr: sources) {
    if(ptr->uuidv4 == uuidv4) {
      this->itemUpdated(ptr);
    }
  }
}

void OverviewModel::itemUpdated(const QSharedPointer<KodiSrc> ptr) {
  auto i = sources.indexOf(ptr);
  if(i < 0) return;

  QModelIndex qmi;
  for(unsigned int c = 0; c != 2; c += 1) {  // update column 0, 1
    qmi = createIndex(i, c);
    emit dataChanged(qmi, qmi);
  }
}

void OverviewModel::addItem(const QSharedPointer<KodiSrc> &ptr) {
  auto i = sources.indexOf(ptr);
  if(i >= 0) {
    // update table
    QModelIndex qmi = createIndex(i, 0);
    emit dataChanged(qmi, qmi);
    return;
  }

  connect(ptr.data(), SIGNAL(updateOverview(QString)), this, SLOT(itemUpdated(QString)));

  // new row
  beginInsertRows(QModelIndex(), 0, 0);
  sources.prepend(ptr);
  endInsertRows();

  QModelIndex qmi = createIndex(0, 0);
  emit dataChanged(qmi, qmi);
}

void OverviewModel::onOverviewRowClicked(uint32_t idx) {
  QSharedPointer<KodiSrc> msg = sources[idx];
  emit overviewRowClicked(msg);
}

QHash<int, QByteArray> OverviewModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[ContentRole] = "content";
  roles[statusIcon] = "msg_status_icon";
  roles[settingsIcon] = "chat_type_icon";
  return roles;
}

void OverviewModel::preloadPixmaps() {
  // /usr/share/icons/hicolor/48x48/hildon/
  //const auto basepath = QDir::currentPath() + "/src/assets/icons/";
  const QStringList icons = {
    ":/assets/monitor_on",
    ":/assets/monitor_off",
    ":/assets/settings"
  };

  for(const auto &icon: icons) {
    const auto fn = QString("%1.png").arg(icon);
    if(!Utils::fileExists(fn))
      continue;
    m_pixmaps[icon] = QPixmap(fn);
  }
}

void OverviewModel::saveToCfg() {
  qDebug() << "saveToCfg";
  QJsonArray arr;
  for(const auto &src: sources) {
    auto obj = src->to_json();
    arr << obj;
  }

  auto dumps = QJsonDocument(arr).toJson(QJsonDocument::Compact);
  config()->set(ConfigKeys::KodiSources, dumps);
}

void OverviewModel::clear() {
  beginResetModel();
  this->sources.clear();
  endResetModel();
}

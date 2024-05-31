#pragma once

#include <QObject>
#include <QProcess>
#include <QHostAddress>
#include <QMainWindow>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QNetworkAddressEntry>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QQueue>
#include <QQmlEngine>
#include <QThread>
#include <QMutex>

#include "overview/OverviewModel.h"
#include "lib/network/network.h"
#include "lib/config.h"


class Ctx : public QObject {
Q_OBJECT

public:
  explicit Ctx(QObject *parent = nullptr);
  ~Ctx() override;

  bool isDebug;
  QString preloadModel;
  QString configDirectory;
  QString configRoot;
  QString homeDir;

  Network *network = nullptr;
  OverviewModel *overviewModel = nullptr;

  void applicationLog(const QString &msg);
  Q_INVOKABLE void singleShot(int msec, QJSValue callback);

private:
  static void createConfigDirectory(const QString &dir);
  void unpackAppArtifacts();
};

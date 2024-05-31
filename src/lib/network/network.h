#pragma once

#include <QObject>
#include <QTimer>
#include <QProcess>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkAddressEntry>
#include <QJsonDocument>

#include "lib/network/httpkodi.h"

class IpData {
Q_GADGET
public:
  QString ipv4;
  QString country;
  QString countryCode;
  QString region;
  QString regionName;
  QString city;
  QString zip;
  double lat;
  double lon;
  QString timezone;
  QString isp;
  QString as;
};

class NetworkIface {
Q_GADGET
public:
  QString name;
  QString address;
  QString netmask;
};

class Network : public QObject
{
Q_OBJECT

public:
  explicit Network(const QString &configDirectory, QObject* parent = nullptr);
  
  QList<QSharedPointer<NetworkIface>> ifaces;
  bool isScanning = false;

  QString getDefaultIfaceScanRange();
  QSharedPointer<NetworkIface> getDefaultIface();
  static bool isPrivateAddress(uint32_t ip);

signals:
  void networkScanError(QString msg);
  void networkScanStart();
  void networkScanFinished();
  void networkScanResult(const QString &scheme, const QString &address, unsigned int port);
  void simplePing(bool status);
  void simplePingError(QProcess::ProcessError);

public slots:
  void onSimplePing(const QString address, unsigned int port, bool tls, const QString &username, const QString &password);

private slots:
  void onScanNetworkFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void onSimplePingFinished(int exitCode, QProcess::ExitStatus exitStatus);

public slots:
  void onScanForKodiInstances();
  void onGetNetworkInterfaces();

private:
  void getNetworkInterfaces();
  QProcess *m_procScanNetwork;
  QProcess *m_procSimplePing;

  QString m_configDirectory;
};


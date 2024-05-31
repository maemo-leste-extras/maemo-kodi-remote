#include <QNetworkReply>
#include <QJsonParseError>

#include "network.h"

Network::Network(const QString &configDirectory, QObject* parent) : 
    m_configDirectory(configDirectory), 
    QObject(parent) {
  // scan network process
  m_procScanNetwork = new QProcess(this);
  connect(m_procScanNetwork , SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onScanNetworkFinished(int, QProcess::ExitStatus)));

  m_procSimplePing = new QProcess(this);
  connect(m_procSimplePing, SIGNAL(finished(int, QProcess::ExitStatus)),   this, SLOT(onSimplePingFinished(int, QProcess::ExitStatus)));
  connect(m_procSimplePing, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SIGNAL(simplePingError(QProcess::ProcessError)));
  this->onGetNetworkInterfaces();
}

void Network::onGetNetworkInterfaces() {
  ifaces.clear();

  QList<QNetworkInterface> allInterfaces = QNetworkInterface::allInterfaces();
  QNetworkInterface eth;

  for(const auto &eth: allInterfaces) {
    // skip inactive, and loopback
    if(!eth.flags().testFlag(QNetworkInterface::IsUp) || eth.flags().testFlag(QNetworkInterface::IsLoopBack))
      continue;

    QList<QNetworkAddressEntry> allEntries = eth.addressEntries();
    QNetworkAddressEntry entry;
    for(const auto &entry: allEntries) {
      auto addr = entry.ip();
      auto ipv4 = static_cast<uint32_t>(addr.toIPv4Address());
      if(!isPrivateAddress(ipv4)) {
        qDebug() << "skipping addr" << addr.toString() << ", not private";
        continue;
      }

      auto iface = new NetworkIface;
      iface->name = eth.name();
      iface->address = addr.toString();
      iface->netmask = entry.netmask().toString();
      QSharedPointer<NetworkIface> ptr(iface);
      qDebug() << "Network interface: " << iface->name;
      ifaces << ptr;
    }
  }
}

void Network::onScanNetworkFinished(int exitCode, QProcess::ExitStatus exitStatus) {
  qDebug() << "finished";
  isScanning = false;
  emit networkScanFinished();

  QString results(m_procScanNetwork->readAllStandardOutput());
  for(const auto &line: results.split('\n')) {
    if(!line.contains(":"))
      continue;

    auto spl = line.split(':');
    auto host = spl.at(0);
    auto port = spl.at(1);
    if(port != "8080")
      continue;

    // validate addr
    QHostAddress address(host);
    if(QAbstractSocket::IPv4Protocol != address.protocol())
      continue;

    emit networkScanResult("http", host, port.toInt());
  }
}

void Network::onSimplePing(const QString address, unsigned int port, bool tls, const QString &username, const QString &password) {
  if(m_procSimplePing->state() != QProcess::ProcessState::NotRunning) {
    qWarning() << "Simple scan busy";
    return;
  }

  QStringList arguments;
  arguments << m_configDirectory + "/" + "kodiping.sh";
  arguments << QString("%1://%2:%3").arg(tls ? "https" : "http", address, QString::number(port));
  if(!username.isEmpty() && !password.isEmpty())
    arguments << QString("%1:%2").arg(username, password);
  qDebug() << arguments;
  m_procSimplePing->start("/bin/bash", arguments);
}

void Network::onSimplePingFinished(int exitCode, QProcess::ExitStatus exitStatus) {
  emit simplePing(exitCode == 0);
}

void Network::onScanForKodiInstances() {
  this->onGetNetworkInterfaces();

  if(isScanning) {
    qDebug() << "Busy scanning";
    emit networkScanError("Busy scanning");
    return;
  }

  isScanning = true;
  emit networkScanStart();

  auto scanRange = this->getDefaultIfaceScanRange();
  if(scanRange.isEmpty()) {
    isScanning = false;
    auto msg = "Could not determine scan range - are you connected to a LAN?";
    qDebug() << msg;
    emit networkScanError(msg);
    emit networkScanFinished();
  }

  QStringList arguments;
  arguments << m_configDirectory + "/" + "kodiscan.sh";
  arguments << scanRange;
  m_procScanNetwork->start("/bin/bash", arguments);
}

QString Network::getDefaultIfaceScanRange() {
  auto iface = this->getDefaultIface();
  if(iface == nullptr) return {};
  auto addr = iface->address;
  auto spl = addr.split(".");
  return QString("%1.%2.%3.1-254").arg(spl.at(0), spl.at(1), spl.at(2));  // yolo
}

QSharedPointer<NetworkIface> Network::getDefaultIface() {
  // could do something smart here, for now just return the first
  if(ifaces.length() <= 0) nullptr;
  return ifaces.at(0);
  // auto iface_default = ifaces.at(0);
  // for(const auto iface: ifaces) {}
}

bool Network::isPrivateAddress(uint32_t ip) {
  uint8_t b1, b2, b3, b4;
  b1 = (uint8_t)(ip >> 24);
  b2 = (uint8_t)((ip >> 16) & 0x0ff);
  b3 = (uint8_t)((ip >> 8) & 0x0ff);
  b4 = (uint8_t)(ip & 0x0ff);

  // 10.x.y.z
  if (b1 == 10)
    return true;
  // 172.16.0.0 - 172.31.255.255
  if ((b1 == 172) && (b2 >= 16) && (b2 <= 31))
    return true;
  // 192.168.0.0 - 192.168.255.255
  if ((b1 == 192) && (b2 == 168))
    return true;
  return false;
}
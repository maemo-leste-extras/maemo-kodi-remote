#include "eventserver.h"

#include <QUdpSocket>
#include <QtEndian>
#include <QDebug>

namespace {
// EventServer packet types
constexpr unsigned short PT_BUTTON = 3;

// BUTTON flags
constexpr unsigned short BT_USE_NAME  = 0x01;
constexpr unsigned short BT_DOWN      = 0x02;
constexpr unsigned short BT_UP        = 0x04;
constexpr unsigned short BT_NO_REPEAT = 0x20;
constexpr unsigned short BT_QUEUE     = 0x10;

constexpr unsigned short VERSION_MAJOR = 2;
constexpr unsigned short VERSION_MINOR = 0;

const QByteArray SIGNATURE = "XBMC";

// uid is an arbitrary 32-bit client id; any stable value is fine.
constexpr unsigned int UID = 0x4b4f4449; // "KODI"
}

EventServer::EventServer(QObject *parent) :
    QObject(parent),
    m_socket(new QUdpSocket(this))
{
}

void EventServer::setAddress(const QString &address, unsigned short port) {
  m_address = QHostAddress(address);
  m_port = port;
}

void EventServer::putU16(QByteArray &out, unsigned short v) {
  char buf[2];
  qToBigEndian<unsigned short>(v, buf);
  out.append(buf, 2);
}

void EventServer::putU32(QByteArray &out, unsigned int v) {
  char buf[4];
  qToBigEndian<unsigned int>(v, buf);
  out.append(buf, 4);
}

QByteArray EventServer::formatString(const QString &str) {
  QByteArray out = str.toUtf8();
  out.append('\0');
  return out;
}

QByteArray EventServer::header(unsigned short packetType, unsigned short packetSize, unsigned int seq) const {
  QByteArray out;
  out.append(SIGNATURE);
  out.append(static_cast<char>(VERSION_MAJOR));
  out.append(static_cast<char>(VERSION_MINOR));
  putU16(out, packetType);
  putU32(out, seq);        // sequence number
  putU32(out, seq);        // max sequence number
  putU16(out, packetSize); // payload size
  putU32(out, UID);
  for(int i = 0; i < 10; ++i) // 10 reserved bytes
    out.append('\0');
  return out;
}

QByteArray EventServer::buttonPacket(const QString &keyname, bool down) const {
  QByteArray payload;
  putU16(payload, 0); // code (unused; we use name)
  unsigned short flags = BT_USE_NAME | BT_NO_REPEAT | BT_QUEUE | (down ? BT_DOWN : BT_UP);
  putU16(payload, flags);
  putU16(payload, 0); // amount
  payload.append(formatString("KB"));
  payload.append(formatString(keyname));
  return payload;
}

void EventServer::send(const QByteArray &payload) {
  QByteArray packet = header(PT_BUTTON, static_cast<unsigned short>(payload.size()), m_seq);
  packet.append(payload);
  m_socket->writeDatagram(packet, m_address, m_port);
}

void EventServer::sendKey(const QString &keyname) {
  send(buttonPacket(keyname, true));  // down
  send(buttonPacket(keyname, false)); // up
}

void EventServer::sendEnter() {
  sendKey("enter");
}

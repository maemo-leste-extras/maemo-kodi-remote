#pragma once

#include <QObject>
#include <QHostAddress>
#include <QByteArray>

class QUdpSocket;

// Minimal Kodi EventServer client.
//
// Kodi's EventServer (UDP :9777) accepts raw input events and routes them
// through the same keymap files a physically attached device uses. Sending a
// BUTTON packet with map_name "KB" and a keyboard keyname is therefore
// equivalent to pressing that key on a keyboard plugged into the Kodi box --
// unlike JSON-RPC Input.* methods, which inject semantic *actions* and bypass
// the per-window keymap.
class EventServer : public QObject
{
Q_OBJECT

public:
  explicit EventServer(QObject *parent = nullptr);

  void setAddress(const QString &address, unsigned short port = 9777);

  // Press+release a keyboard key by its Kodi keyname (e.g. "enter", "tab", "o").
  void sendKey(const QString &keyname);

  // Convenience wrappers for the Enter key, which Kodi's keyboard.xml maps to
  // the active window's Select action (video OSD controls / item activation).
  void sendEnter();

private:
  QByteArray buttonPacket(const QString &keyname, bool down) const;
  QByteArray header(unsigned short packetType, unsigned short packetSize, unsigned int seq) const;
  void send(const QByteArray &payload);

  static QByteArray formatString(const QString &str);
  static void putU16(QByteArray &out, unsigned short v);
  static void putU32(QByteArray &out, unsigned int v);

  QUdpSocket *m_socket = nullptr;
  QHostAddress m_address = QHostAddress::LocalHost;
  unsigned short m_port = 9777;
  unsigned int m_seq = 1;
};

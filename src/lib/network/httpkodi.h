#pragma once

#include <QObject>
#include <QTimer>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonDocument>

class HttpKodi : public QObject
{
Q_OBJECT

public:
  explicit HttpKodi(const QString &uid, const QString &url, QObject* parent = nullptr);
  void setCredentials(const QString &username, const QString &password);

  QString name;

  void post(const QString &uid, const QString &method, const QJsonObject &params);
  void post(const QString &uid, const QString &method, unsigned int num);
  void _post(const QString &uid, const QString &method, const QJsonDocument &doc);

signals:
  void requestComplete(const QJsonDocument &response);
  void requestFailed(const QString &errorString);

public slots:
  void onUrlChanged(QString url, QString username, QString password);

private slots:
  void requestTimeout();
  void handleJSONReply();

private:
  QNetworkAccessManager *m_networkManager;
  QNetworkReply *m_reply = nullptr;
  QTimer *m_timer;

  QString m_url;
  QString m_username;
  QString m_password;

  bool m_busy = false;
};


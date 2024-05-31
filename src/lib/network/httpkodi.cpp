#include <QNetworkReply>
#include <QJsonParseError>

#include "httpkodi.h"

HttpKodi::HttpKodi(const QString &name, const QString &url, QObject* parent) : 
    QObject(parent), 
    m_url(url),
    name(name),
    m_networkManager(new QNetworkAccessManager(this))
{
  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, this, &HttpKodi::requestTimeout);
}

void HttpKodi::post(const QString &uid, const QString &method, const QJsonObject &params){
  QJsonObject obj;
  obj["jsonrpc"] = "2.0";
  obj["method"] = method;
  obj["id"] = uid;

  if(!params.empty())
    obj["params"] = params;

  QJsonDocument doc(obj);
  this->_post(uid, method, doc);
}

void HttpKodi::post(const QString &uid, const QString &method, unsigned int num) {
  QJsonObject obj;
  obj["jsonrpc"] = "2.0";
  obj["method"] = method;
  obj["id"] = uid;
  obj["params"] = double(num);
  QJsonDocument doc(obj);
  this->_post(uid, method, doc);
}

void HttpKodi::_post(const QString &uid, const QString &method, const QJsonDocument &doc) {
  m_busy = true;
  QNetworkRequest request(m_url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  if(!m_username.isEmpty() && !m_password.isEmpty()){
    QString creds = m_username + ":" + m_password;
    QByteArray data = creds.toLocal8Bit().toBase64();
    QString authHeader = "Basic " + data;
    request.setRawHeader("Authorization", authHeader.toLocal8Bit());
  }

  QByteArray data = doc.toJson();
  QNetworkReply *reply = m_networkManager->post(request, data);

  m_timer->setInterval(2000);
  m_timer->start();

  connect(reply, &QNetworkReply::finished, this, &HttpKodi::handleJSONReply);
}

void HttpKodi::setCredentials(const QString &username, const QString &password) {
  m_username = username;
  m_password = password;
}

void HttpKodi::requestTimeout() { // is this needed?
  if(m_reply != nullptr) {
    m_reply->abort();
    m_reply->deleteLater();
    m_reply = nullptr;
  }

  m_busy = false;
  emit requestFailed("timeout error");
}

void HttpKodi::handleJSONReply() {
  m_timer->stop();
  m_busy = false;

  auto *reply = qobject_cast<QNetworkReply*>(sender());
  if (reply && reply->error() == QNetworkReply::NoError) {
    QByteArray responseData = reply->readAll();
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(responseData, &error);
    if (error.error == QJsonParseError::NoError) {
      emit requestComplete(jsonDocument);
    } else {
      qDebug() << "handleJSONReply err 2";
      emit requestFailed(error.errorString());
    }
  } else {
    // Handle error
    qDebug() << "handleJSONReply err 3";
    emit requestFailed(reply->errorString());
  }
  reply->deleteLater();
}

void HttpKodi::onUrlChanged(QString url, QString username, QString password) {
  m_url = url;
  this->setCredentials(username, password);
}

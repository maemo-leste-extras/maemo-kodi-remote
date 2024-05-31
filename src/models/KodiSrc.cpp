#include "KodiSrc.h"

KodiSrc::KodiSrc(const QString &name, const QString &address, const QString &scheme, unsigned int port, const QString &username, const QString &password, QObject *parent) : 
    name(name),
    address(address),
    port(port),
    scheme(scheme),
    username(username),
    password(password),
    nowPlaying(new KodiNowPlaying(this)),
    player(new KodiPlayer(this)),
    m_pingTimer(new QTimer(this)),
    m_nowPlayingTimer(new QTimer(this)),
    m_PlayerStateTimer(new QTimer(this)),
    m_volumeTimer(new QTimer(this)),
    QObject(parent) {
  this->uuidv4 = Utils::generate_uuidv4();

  connect(nowPlaying, &KodiNowPlaying::displayNameChanged, [this]{ emit updateOverview(uuidv4); });
  connect(this, &KodiSrc::statusIntChanged, [this] { emit updateOverview(uuidv4); });

  // commands
  m_http = new HttpKodi("m_http", this->urlRPC());
  m_http->setCredentials(username, password);
  connect(this, &KodiSrc::kodiURLChanged, m_http, &HttpKodi::onUrlChanged);

  m_httpPing = new HttpKodi("m_httpPing", this->urlRPC());
  connect(this, &KodiSrc::kodiURLChanged, [this](QString url, QString username, QString password) {
    m_httpPing->onUrlChanged(url, username, password);
    this->onPing();
  });

  m_httpPing->setCredentials(username, password);
  connect(m_pingTimer, &QTimer::timeout, this, &KodiSrc::onPing);
  connect(m_httpPing, &HttpKodi::requestComplete, this, &KodiSrc::onPong);
  connect(m_httpPing, &HttpKodi::requestFailed, this, &KodiSrc::onPongFailed);
  m_pingTimer->setInterval(m_pingInterval);
  m_pingTimer->start();
  this->onPing();

  // Player.state (progress)
  m_httpPlayerState = new HttpKodi("m_httpPlayerState", this->urlRPC());
  connect(this, &KodiSrc::kodiURLChanged, m_httpPlayerState, &HttpKodi::onUrlChanged);

  m_httpPlayerState->setCredentials(username, password);
  connect(m_PlayerStateTimer, &QTimer::timeout, this, &KodiSrc::onPlayerState);
  connect(m_httpPlayerState, &HttpKodi::requestComplete, this, &KodiSrc::onPlayerStateFinished);
  connect(m_httpPlayerState, &HttpKodi::requestFailed, this, &KodiSrc::onPongFailed);
  m_PlayerStateTimer->setInterval(m_playerStateInterval);
  m_PlayerStateTimer->start();

  // Application properties (volume, name)
  m_httpVolume = new HttpKodi("m_httpVolume", this->urlRPC());
  connect(this, &KodiSrc::kodiURLChanged, m_httpVolume, &HttpKodi::onUrlChanged);

  m_httpVolume->setCredentials(username, password);
  connect(m_volumeTimer, &QTimer::timeout, this, &KodiSrc::onGetVolume);
  connect(m_httpVolume, &HttpKodi::requestComplete, this, &KodiSrc::onVolumeFinished);
  connect(m_httpVolume, &HttpKodi::requestFailed, this, &KodiSrc::onPongFailed);
  m_volumeTimer->setInterval(m_volumeInterval);
  m_volumeTimer->start();

  // now playing
  m_httpNowPlaying = new HttpKodi("m_httpNowPlaying", this->urlRPC());
  connect(this, &KodiSrc::kodiURLChanged, m_httpNowPlaying, &HttpKodi::onUrlChanged);
  m_httpNowPlaying->setCredentials(username, password);
  connect(m_nowPlayingTimer, &QTimer::timeout, this, &KodiSrc::onGetPlayer);
  connect(m_httpNowPlaying, &HttpKodi::requestComplete, this, &KodiSrc::onGetPlayerOrNowPlayingFinished);
  connect(m_httpNowPlaying, &HttpKodi::requestFailed, this, &KodiSrc::onPongFailed);
  m_nowPlayingTimer->setInterval(m_nowPlayingInterval);
  m_nowPlayingTimer->start();

  if(this->name.isEmpty())
    this->name = QString("%1:%2").arg(address, QString::number(port));
}

void KodiSrc::send(const QString &cmd) {
  qDebug() << "send" << cmd;
  m_http->post(QString("send.%1").arg(cmd), cmd, QJsonObject());
}

void KodiSrc::sendText(QString msg) {
  msg = msg.trimmed();
  qDebug() << "Input.SendText" << msg;
  QJsonObject params;
  params["text"] = msg;
  params["done"] = true;
  m_http->post("Input.SendText", "Input.SendText", params);
}

void KodiSrc::onGetVolume() {
  if(status < KodiStatus::CHILLING || m_playerID < 0) 
    return;

  QJsonObject params;
  QStringList props({
      "name",
      "volume",
      "muted"
  });

  auto properties = QJsonArray::fromStringList(props);
  params["properties"] = properties;

  m_httpVolume->post("Application.GetProperties", "Application.GetProperties", params);
}

void KodiSrc::onVolumeFinished(const QJsonDocument& response) {
  auto doc = response.object();
  auto obj = doc["result"].toObject();
  unsigned int _volume = obj["volume"].toInt();
  bool _volume_muted = obj["muted"].toBool();

  if(this->volume != _volume) {
    this->volume = _volume;
    emit volumeChanged();
  }

  if(this->volume_muted != _volume_muted) {
    this->volume_muted = _volume_muted;
    emit volumeMutedChanged();
  }
}

void KodiSrc::onPlayerState() {
  if(status < KodiStatus::CHILLING || m_playerID < 0) 
    return;

  QJsonObject params;
  params["playerid"] = double(m_playerID);

  QStringList props({
      "canmove",
      "canseek",
      "live",
      "canrepeat",
      "canchangespeed",
      "percentage",
      "position",
      "repeat",
      "shuffled",
      "speed",
      "time",
      "totaltime",
      "currentsubtitle",
      "audiostreams",
      "subtitles"
  });

  auto properties = QJsonArray::fromStringList(props);
  params["properties"] = properties;

  m_httpPlayerState->post("Player.GetProperties", "Player.GetProperties", params);
}

void KodiSrc::onPlayerStateFinished(const QJsonDocument& response) {
  auto doc = response.object();
  auto obj = doc["result"].toObject();

  player->setCanmove(obj["canmove"].toBool());
  player->setCanseek(obj["canseek"].toBool());
  player->setLive(obj["live"].toBool());
  player->setCanrepeat(obj["canrepeat"].toBool());
  player->setCanchangespeed(obj["canchangespeed"].toBool());
  player->setRepeat(obj["repeat"].toBool());
  player->setShuffled(obj["shuffled"].toBool());
  player->setPercentage(obj["percentage"].toDouble());
  player->setPosition(obj["position"].toInt());
  player->setSpeed(obj["speed"].toDouble());

  QStringList chans;
  for(const auto &chan: obj["audiostreams"].toArray()) {
    auto chan_obj = chan.toObject();
    auto chan_name = chan_obj["name"].toString();
    auto chan_lang = chan_obj["language"].toString();
    if(chan_name.isEmpty())
      chans << chan_lang;
    else
      chans << QString("%1 | %2").arg(chan_lang, chan_name);
  }
  player->setAudioStreams(chans);

  QStringList subs;
  for(const auto &sub: obj["subtitles"].toArray()) {
    auto sub_obj = sub.toObject();
    auto sub_name = sub_obj["name"].toString();
    auto sub_lang = sub_obj["language"].toString();
    if(sub_name.isEmpty())
      subs << sub_lang;
    else
      subs << QString("%1 | %2").arg(sub_lang, sub_name);
  }
  player->setSubtitles(subs);

  auto time = obj["time"].toObject();
  if(!time.empty()) {
    unsigned int secs = 0;
    secs += time["hours"].toInt() * 3600;
    secs += time["minutes"].toInt() * 60;
    secs += time["seconds"].toInt();
    player->setTime(secs);
  } else {
    player->setTime(0);
  }

  auto totaltime = obj["totaltime"].toObject();
  if(!totaltime.empty()) {
    auto secs = 0;
    secs += totaltime["hours"].toInt() * 3600;
    secs += totaltime["minutes"].toInt() * 60;
    secs += totaltime["seconds"].toInt();
    player->setTotaltime(secs);
  } else {
    player->setTotaltime(0);
  }

  auto sub = obj["currentsubtitle"].toObject();
  if(!sub.empty()) {
    player->setCurrentsubtitle(sub["language"].toString());
  } else {
    player->setCurrentsubtitle("");
  }
}

void KodiSrc::onPing() {
  m_httpPing->post("ping", "JSONRPC.Ping", QJsonObject());
}

void KodiSrc::onPong(const QJsonDocument& response) {
  if(status == KodiStatus::NO_ROUTE || status == KodiStatus::NO_AUTH) {
    setStatus(KodiStatus::CHILLING);
    if(!m_init) {
      m_init = true;
      this->onGetPlayer();
      this->onPlayerState();
      this->onGetVolume();
    }
  }
}

void KodiSrc::onPongFailed(const QString &error) {
  qDebug() << "onPongFailed";
  if(error.contains("requires authentication")) {
    this->setStatus(KodiStatus::NO_AUTH);
  } else {
    this->setStatus(KodiStatus::NO_ROUTE);
  }
}

void KodiSrc::setStatus(KodiStatus _status) {
  if(_status == status) return;
  qDebug() << "setStatus" << _status;

  status = _status;
  emit statusChanged(status);
  emit statusIntChanged();
}

void KodiSrc::onGetPlayer() {
  if(status < KodiStatus::CHILLING) return;
  m_httpNowPlaying->post("GetActivePlayers", "Player.GetActivePlayers", QJsonObject());
}

void KodiSrc::onNowPlaying() {
  if(status < KodiStatus::CHILLING) return;

  QJsonObject params;
  params["playerid"] = double(m_playerID);

  QStringList props({
      "title",
      "album",
      "artist",
      "season",
      "episode",
      "duration",
      "showtitle",
      "tvshowid",
      "thumbnail",
      "file",
      "fanart",
      "streamdetails"
  });

  auto properties = QJsonArray::fromStringList(props);
  params["properties"] = properties;

  m_httpNowPlaying->post("np", "Player.GetItem", params);
}

void KodiSrc::onGetPlayerOrNowPlayingFinished(const QJsonDocument& response) {
  auto obj = response.object();
  if(!obj.contains("id") || !obj.contains("result")) {
    qDebug() << "'id' or 'result' not present in blob" << response.toJson();
    return;
  }

  auto uid = obj["id"].toString();

  if(uid == "GetActivePlayers") {
    auto arr = obj["result"].toArray();
    auto _obj = arr.at(0).toObject();
    m_playerID = _obj["playerid"].toInt();
    this->onNowPlaying();
  } else {
    auto _obj = obj["result"].toObject();
    auto item = _obj["item"].toObject();
    auto _media_type = item["type"].toString();

    KodiNowPlayingType npType;
    if(_media_type == "movie") {
      npType = KodiNowPlayingType::MOVIE;
    } else if(_media_type == "episode") {
      npType = KodiNowPlayingType::EPISODE;
    } else if(_media_type == "song") {
      npType = KodiNowPlayingType::SONG;
    } else {
      npType = KodiNowPlayingType::UNKNOWN;
    }

    if(npType == KodiNowPlayingType::UNKNOWN) 
      setStatus(KodiStatus::CHILLING);
    else
      setStatus(KodiStatus::PLAYING);

    auto subs = item["subtitles"].toArray();
    QStringList subtitles;
    for(const auto &sub: subs) {
      auto _sub = sub.toObject();
      subtitles << _sub["language"].toString();
    }

    auto album = item["album"].toString();
    auto fanart = item["fanart"].toString();
    auto file = item["file"].toString();
    auto label = item["label"].toString();
    auto artist = item["artist"].toString();
    auto showtitle = item["showtitle"].toString();
    auto thumbnail = item["thumbnail"].toString();
    if(thumbnail.endsWith("/"))
      thumbnail.truncate(thumbnail.lastIndexOf(QChar('/')));

    auto title = item["title"].toString();

    unsigned int duration = 0;
    if(item.contains("streamdetails")) {
      auto streamdetails = item["streamdetails"].toObject();
      QString key;
      if(streamdetails.contains("video")) {
        auto video = streamdetails["video"].toArray();
        auto video_item = video.at(0).toObject();
        duration = video_item["duration"].toInt();
      }
    }

    nowPlaying->setTitle(title);
    nowPlaying->setThumbnail(thumbnail);
    nowPlaying->setFanart(fanart);
    nowPlaying->setFile(file);
    nowPlaying->setLabel(label);
    nowPlaying->setArtist(artist);
    nowPlaying->setAlbum(album);
    nowPlaying->setShowtitle(showtitle);
    nowPlaying->setSubtitles(subtitles);
    nowPlaying->setNptype(npType);
    nowPlaying->setDuration(duration);
  }
}

void KodiSrc::setMute(bool status) {
  // Application.SetMute
  QJsonObject params;
  params["mute"] = status;
  this->m_http->post("setMute", "Application.SetMute", params);
}

void KodiSrc::setVolume(unsigned int pct) {
  QJsonObject params;
  params["volume"] = double(pct);
  this->m_http->post("setVolume", "Application.SetVolume", params);
  this->onGetVolume();
}

void KodiSrc::toggleMute() {
  if(volume_muted) {
    this->setMute(false);
    volume_muted = false;
  } else {
    this->setMute(true);
    volume_muted = true;
  }

  emit volumeMutedChanged();
  this->onGetVolume();
}

void KodiSrc::playPause() {
  this->player->paused = !this->player->paused;
  qDebug() << "playPause setting to" << this->player->paused;
  emit this->player->pausedChanged();

  QJsonObject params;
  params["playerid"] = m_playerID;
  this->m_http->post("Player.PlayPause", "Player.PlayPause", params);

  this->onPlayerState();
}

void KodiSrc::setAudioStream(int idx) {
  QJsonObject params;
  params["playerid"] = m_playerID;
  params["stream"] = idx;
  this->m_http->post("Player.SetAudioStream", "Player.SetAudioStream", params);
}

void KodiSrc::setSubtitle(int idx) {
  QJsonObject params;
  params["playerid"] = m_playerID;
  params["subtitle"] = idx;
  params["enable"] = true;
  this->m_http->post("Player.SetSubtitle", "Player.SetSubtitle", params);
}

void KodiSrc::playerStop() {
  QJsonObject params;
  params["playerid"] = m_playerID;
  this->m_http->post("Player.Stop", "Player.Stop", params);
  this->onPlayerState();
}

QJsonObject KodiSrc::to_json() {
  QJsonObject obj;
  obj["name"] = this->name;
  obj["address"] = this->address;
  obj["port"] = QString::number(this->port);
  obj["username"] = this->username;
  obj["password"] = this->password;
  obj["scheme"] = this->scheme;
  return obj;
}

void KodiSrc::generateOverviewItemDelegateRichText(){
  // Stylesheet: overview/overviewRichDelegate.css
  QString connection;
  if(status == KodiStatus::NO_ROUTE) {
    connection = "Not connected";
  } else if(status == KodiStatus::NO_AUTH) {
    connection = "Bad credentials";
  } else if(status >= KodiStatus::CHILLING) {
    connection = "Connected";
  }

  auto richtext = QString("<span class=\"header\">%1</b>").arg(name);
  richtext += QString("<span class=\"small\">&nbsp;&nbsp;%1</span>").arg(nowPlaying->displayName());
  richtext += QString("<span class=\"small text-muted\">&nbsp;&nbsp;%1:%2</span>").arg(address, QString::number(port));
  richtext += "<br>";
  richtext += QString("<span class=\"text-muted\">%1</span>").arg(connection);

  overviewItemDelegateRichText = richtext;
}

KodiSrc::~KodiSrc() {
  qDebug() << "~KodiSrc ";
}

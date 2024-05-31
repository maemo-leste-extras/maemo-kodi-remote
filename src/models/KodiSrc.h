#pragma once
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>

#include "lib/utils.h"
#include "lib/network/httpkodi.h"

enum KodiNowPlayingType {
    MOVIE = 0,
    EPISODE,
    SONG,
    UNKNOWN
};
Q_DECLARE_METATYPE(KodiNowPlayingType)

enum KodiStatus {
    NO_ROUTE = 0,
    NO_AUTH,
    CHILLING,
    PLAYING
};
Q_DECLARE_METATYPE(KodiStatus)

class KodiPlayer : public QObject
{
Q_OBJECT
Q_PROPERTY(bool canmove MEMBER canmove NOTIFY canmoveChanged);
Q_PROPERTY(bool canseek MEMBER canseek NOTIFY canseekChanged);
Q_PROPERTY(bool live MEMBER live NOTIFY liveChanged);
Q_PROPERTY(bool canrepeat MEMBER canrepeat NOTIFY canrepeatChanged);
Q_PROPERTY(bool canchangespeed MEMBER canchangespeed NOTIFY canchangespeedChanged);
Q_PROPERTY(double percentage MEMBER percentage NOTIFY percentageChanged);
Q_PROPERTY(unsigned int position MEMBER position NOTIFY positionChanged);
Q_PROPERTY(bool repeat MEMBER repeat NOTIFY repeatChanged);
Q_PROPERTY(bool shuffled MEMBER shuffled NOTIFY shuffledChanged);
Q_PROPERTY(double speed MEMBER speed NOTIFY speedChanged);
Q_PROPERTY(unsigned int time MEMBER time NOTIFY timeChanged);
Q_PROPERTY(unsigned int totaltime MEMBER totaltime NOTIFY totaltimeChanged);
Q_PROPERTY(QString currentsubtitle MEMBER currentsubtitle NOTIFY currentsubtitleChanged);
Q_PROPERTY(QString time_str MEMBER time_str NOTIFY timeChanged);
Q_PROPERTY(QStringList audioStreams MEMBER audioStreams NOTIFY audioStreamsChanged);
Q_PROPERTY(QStringList subtitles MEMBER subtitles NOTIFY subtitlesChanged);
Q_PROPERTY(bool paused MEMBER paused NOTIFY pausedChanged);

public:
  explicit KodiPlayer(QObject *parent = nullptr) {}

  QStringList audioStreams;
  QStringList subtitles;
  bool canmove = false;
  bool canseek = false;
  bool live = false;
  bool paused = true;  // derived from speed
  bool canrepeat = false;
  bool canchangespeed = false;
  double percentage;
  unsigned int position;
  bool repeat = false;
  bool shuffled = false;
  double speed = 1.0;
  unsigned int time = 0;
  unsigned int totaltime = 0;
  QString currentsubtitle;
  QString time_str;

  void setSubtitles(QStringList subs) {
    if(this->subtitles == subs)
      return;

    this->subtitles = subs;
    qDebug() << "subtitles available" << subs;
    emit subtitlesChanged();
  }

  void setAudioStreams(QStringList streams) {
    if(this->audioStreams == streams)
      return;

    this->audioStreams = streams;
    qDebug() << "audiostreams" << streams;
    emit audioStreamsChanged();
  }

  void setCanmove(bool status) {
    bool dirty = canmove != status;
    this->canmove = status;
    if(dirty)
      emit canmoveChanged();
  }

  void setCanseek(bool status) {
    bool dirty = canseek != status;
    this->canseek = status;
    if(dirty)
      emit canseekChanged();
  }

  void setLive(bool status) {
    bool dirty = live != status;
    this->live = status;
    if(dirty)
      emit liveChanged();
  }

  void setCanrepeat(bool status) {
    bool dirty = canrepeat != status;
    this->canrepeat = status;
    if(dirty)
      emit canrepeatChanged();
  }

  void setCanchangespeed(bool status) {
    bool dirty = canchangespeed != status;
    this->canchangespeed = status;
    if(dirty)
      emit canchangespeedChanged();
  }

  void setPercentage(double pct) {
    bool dirty = percentage != pct;
    this->percentage = pct;
    if(dirty)
      emit percentageChanged();
  }

  void setPosition(unsigned int pos) {
    bool dirty = position != pos;
    this->position = pos;
    if(dirty)
      emit positionChanged();
  }

  void setRepeat(bool status) {
    auto dirty = repeat != status;
    this->repeat = status;
    if(dirty)
      emit repeatChanged();
  }

  void setShuffled(bool status) {
    bool dirty = shuffled != status;
    this->shuffled = status;
    if(dirty)
      emit shuffledChanged();
  }

  void setSpeed(double num) {
    bool dirty = speed != num;
    this->speed = speed;
    if(dirty)
      emit speedChanged();

    auto _paused = num == 0;
    if(paused != _paused) {
      paused = _paused;
      emit pausedChanged();
    }
  }

  void setTime(unsigned int _time) {
    this->time = _time;
    time_str = QDateTime::fromTime_t(_time).toUTC().toString("hh:mm:ss");
    emit timeChanged();
  }

  void setTotaltime(unsigned int _time) {
    bool dirty = totaltime != _time;
    this->totaltime = _time;
    if(dirty)
      emit totaltimeChanged();
  }

  void setCurrentsubtitle(QString sub) {
    bool dirty = currentsubtitle != sub;
    this->currentsubtitle = sub;
    if(dirty)
      emit currentsubtitleChanged();
  }

signals:
  void canmoveChanged();
  void canseekChanged();
  void liveChanged();
  void canrepeatChanged();
  void canchangespeedChanged();
  void percentageChanged();
  void positionChanged();
  void repeatChanged();
  void shuffledChanged();
  void speedChanged();
  void timeChanged();
  void totaltimeChanged();
  void currentsubtitleChanged();
  void audioStreamsChanged();
  void subtitlesChanged();
  void pausedChanged();
};

class KodiNowPlaying : public QObject
{
Q_OBJECT
Q_PROPERTY(QString title MEMBER title NOTIFY titleChanged);
Q_PROPERTY(QString thumbnail MEMBER thumbnail NOTIFY thumbnailChanged);
Q_PROPERTY(QString fanart MEMBER fanart NOTIFY fanartChanged);
Q_PROPERTY(QString file MEMBER file NOTIFY fileChanged);
Q_PROPERTY(QString label MEMBER label NOTIFY labelChanged);
Q_PROPERTY(QString artist MEMBER artist NOTIFY artistChanged);
Q_PROPERTY(QString album MEMBER album NOTIFY albumChanged);
Q_PROPERTY(QString showtitle MEMBER showtitle NOTIFY showtitleChanged);
Q_PROPERTY(QStringList subtitles MEMBER subtitles NOTIFY subtitlesChanged);
Q_PROPERTY(unsigned int duration MEMBER duration NOTIFY durationChanged);
Q_PROPERTY(QString displayName READ displayName NOTIFY displayNameChanged);
Q_PROPERTY(unsigned int npTypeInt READ npTypeInt NOTIFY npTypeChanged);
Q_PROPERTY(QString duration_str MEMBER duration_str NOTIFY durationChanged);

public:
  explicit KodiNowPlaying(QObject *parent = nullptr) {};

  KodiNowPlayingType npType;
  QString title;
  QString thumbnail;
  QString fanart;
  QString file;
  QString label;
  QString album;
  QString showtitle;
  QString artist;
  QStringList subtitles;
  unsigned int duration = 0;
  QString duration_str;

  unsigned int npTypeInt() {
    return static_cast<unsigned int>(npType);
  }

  void setNptype(KodiNowPlayingType _npType) {
    bool dirty = _npType != this->npType;
    this->npType = _npType;

    if(dirty)
      emit npTypeChanged();
  }

  void setTitle(const QString &_title) {
    bool dirty = _title != this->title;
    this->title = _title;

    if(dirty) {
      emit titleChanged();
      emit displayNameChanged();
    }
  }

  void setThumbnail(QString _thumbnail) {
    if(_thumbnail.startsWith("image://")) {
      _thumbnail = _thumbnail.remove(0, 8); 
      _thumbnail = QString(QUrl::fromPercentEncoding(_thumbnail.toUtf8()));
    }

    bool dirty = _thumbnail != this->thumbnail;
    this->thumbnail = _thumbnail;

    if(dirty)
      emit thumbnailChanged();
  }

  void setFanart(const QString &_fanart) {
    bool dirty = _fanart != this->fanart;
    this->fanart = _fanart;

    if(dirty)
      emit fanartChanged();
  }

  void setFile(const QString &_file) {
    bool dirty = _file != this->file;
    this->file = _file;

    if(dirty) {
      emit fileChanged();
      emit displayNameChanged();
    }
  }

  void setLabel(const QString &_label) {
    bool dirty = _label != this->label;
    this->label = _label;

    if(dirty) {
      emit labelChanged();
      emit displayNameChanged();
    }
  }

  void setArtist(const QString &_artist) {
    bool dirty = _artist != this->artist;
    this->artist = _artist;

    if(dirty) {
      emit artistChanged();
      emit displayNameChanged();
    }
  }

  void setAlbum(const QString &_album) {
    bool dirty = _album != this->album;
    this->album = _album;

    if(dirty)
      emit albumChanged();
  }

  void setShowtitle(const QString &_showtitle) {
    bool dirty = _showtitle != this->showtitle;
    this->showtitle = _showtitle;

    if(dirty)
      emit showtitleChanged();
  }

  void setSubtitles(const QStringList &_subtitles) {
    bool dirty = _subtitles != this->subtitles;
    this->subtitles = _subtitles;

    if(dirty)
      emit subtitlesChanged();
  }

  void setDuration(unsigned int _duration) {
    bool dirty = _duration != this->duration;
    this->duration = _duration;

    if(dirty) {
      duration_str = QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss");
      emit durationChanged();
    }
  }

  QString displayName() {
    QString name;
    if(!label.isEmpty())
      name = label;
    else if(!title.isEmpty())
      name = title;
    else if(!file.isEmpty())
      name = file;
    else
      name = "idle";

    if(!showtitle.isEmpty())
      name = QString("%1: %2").arg(showtitle, name);

    if(name.length() >= 64)
      name = name.mid(0, 64) + "...";

    name = name.trimmed();
    return name;
  }

signals:
  void titleChanged();
  void thumbnailChanged();
  void fanartChanged();
  void fileChanged();
  void labelChanged();
  void artistChanged();
  void albumChanged();
  void showtitleChanged();
  void subtitlesChanged();
  void durationChanged();
  void npTypeChanged();
  void displayNameChanged();
};

class KodiSrc : public QObject
{
Q_OBJECT
Q_PROPERTY(KodiStatus status MEMBER status NOTIFY statusChanged);
Q_PROPERTY(unsigned int statusInt READ statusInt NOTIFY statusIntChanged);
Q_PROPERTY(unsigned int volume MEMBER volume NOTIFY volumeChanged);
Q_PROPERTY(bool volume_muted MEMBER volume_muted NOTIFY volumeMutedChanged);

public:
  explicit KodiSrc(const QString &name, const QString &address, const QString &scheme, unsigned int port, const QString &username, const QString &password, QObject *parent = nullptr);
  ~KodiSrc() override;
  static KodiSrc* fromPath(QFileInfo path);

  QString uuidv4;
  QString name;
  QString scheme = "http";
  QString address;
  unsigned int port;
  QString username;
  QString password;

  bool volume_muted = false;
  unsigned int volume = 100;

  void send(const QString &cmd);
  Q_INVOKABLE void sendText(QString msg);

  Q_INVOKABLE QString urlRPC() {
    return QString("%1://%2:%3/jsonrpc").arg(scheme, address, QString::number(port));
  }

  KodiStatus status = KodiStatus::NO_ROUTE;
  unsigned int statusInt() { return static_cast<unsigned int>(status); }
  unsigned int m_httpThrottleFactor = 1;
  bool windowActive = false;

  KodiPlayer *player;
  KodiNowPlaying *nowPlaying;

  QString overviewItemDelegateRichText;
  void generateOverviewItemDelegateRichText();

  QJsonObject to_json();

  void setStatus(KodiStatus status);

  void setName(QString _name) {
    name = _name;
    emit nameChanged();
    emit updateOverview(uuidv4);
  }

  void setAddress(QString _address) {
    address = _address;
  }

  void setPort(unsigned int _port) {
    port = _port;
  }

  void setScheme(QString _scheme) {
    scheme = _scheme;
  }

  void setCredentials(QString _username, QString _password) {
    this->username = _username;
    this->password = _password;
  }

  void setWindowStatus(bool active) {
    windowActive = active;
    m_httpThrottleFactor = active ? 1 : 2;
    if(active) {
      m_pingTimer->setInterval(m_pingInterval);
      m_PlayerStateTimer->setInterval(m_playerStateInterval);
      m_volumeTimer->setInterval(m_volumeInterval);
      m_nowPlayingTimer->setInterval(m_nowPlayingInterval);
    } else {
      m_pingTimer->setInterval(m_pingInterval * m_httpThrottleFactor);
      m_PlayerStateTimer->setInterval(m_playerStateInterval * m_httpThrottleFactor);
      m_volumeTimer->setInterval(m_volumeInterval * m_httpThrottleFactor);
      m_nowPlayingTimer->setInterval(m_nowPlayingInterval * m_httpThrottleFactor);
    }
  }

  Q_INVOKABLE void setVolume(unsigned int pct);
  Q_INVOKABLE void setMute(bool status);
  Q_INVOKABLE void toggleMute();
  Q_INVOKABLE void playPause();
  Q_INVOKABLE void playerStop();
  Q_INVOKABLE void setAudioStream(int idx);
  Q_INVOKABLE void setSubtitle(int idx);

signals:
  void kodiURLChanged(QString url, QString username, QString password);
  void onlineChanged(bool status);
  void statusChanged(KodiStatus status);
  void statusIntChanged();
  void volumeChanged();
  void volumeMutedChanged();
  void showKeyboard();
  void updateOverview(QString uuidv4);
  void nameChanged();

public slots:
  void onGetPlayer();
  void onNowPlaying();
  void onPlayerState();
  void onGetVolume();

private slots:
  void onPing();
  void onPong(const QJsonDocument &response);
  void onPongFailed(const QString &error);

  void onGetPlayerOrNowPlayingFinished(const QJsonDocument& response);
  void onPlayerStateFinished(const QJsonDocument& response);
  void onVolumeFinished(const QJsonDocument& response);

private:
  bool m_init = false;
  int m_playerID = -1;

  const unsigned int m_pingInterval = 3000;
  const unsigned int m_playerStateInterval = 1000;
  const unsigned int m_volumeInterval = 2000;
  const unsigned int m_nowPlayingInterval = 3000;

  HttpKodi *m_http;

  HttpKodi *m_httpPing;
  QTimer *m_pingTimer;

  HttpKodi *m_httpPlayerState;
  QTimer *m_PlayerStateTimer;

  HttpKodi *m_httpVolume;
  QTimer *m_volumeTimer;

  HttpKodi *m_httpNowPlaying;
  QTimer *m_nowPlayingTimer;
};

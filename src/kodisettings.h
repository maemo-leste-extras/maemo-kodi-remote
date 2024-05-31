#pragma once
#include <QtGlobal>
#include <QResource>
#include <QApplication>
#include <QScreen>
#include <QHostAddress>
#include <QtWidgets/QMenu>
#include <QMainWindow>
#include <QObject>
#include <QtCore>
#include <QtGui>
#include <QFileInfo>

#include <iostream>

#include "ctx.h"
#include "models/KodiSrc.h"
#include "lib/config.h"

namespace Ui {
    class KodiSettings;
}

class KodiSettingsForm {
Q_GADGET
public:
  QString name;
  QString address;
  unsigned int port;
  QString username;
  QString password;
  QString scheme;
  QString errorMsg;

  bool valid() {
    if(name.isEmpty()) {
      errorMsg = "Name cannot be empty";
      return false;
    }
    address = address.trimmed();
    if(address.isEmpty()) {
      errorMsg = "Address cannot be empty";
      return false;
    }
    if(port <= 0 || port > 65535) {
      errorMsg = "Port must be between 1 and 65535";
      return false;
    }
    if(scheme.isEmpty()) {
      errorMsg = "Scheme cannot be empty";
      return false;
    }
    if(!username.isEmpty() && password.isEmpty()) {
      errorMsg = "Password cannot be empty";
      return false;
    }
    if(username.isEmpty() && !password.isEmpty()) {
      errorMsg = "Username cannot be empty";
      return false;
    }
    if(password.contains(":")) {
      errorMsg = "Do not use the : character in the password, it is not supported (and I am a lazy programmer)";
      return false;
    }
    return true;
  }
};

class KodiSettings : public QMainWindow {
Q_OBJECT

public:
  explicit KodiSettings(Ctx *ctx, QSharedPointer<KodiSrc> mdl, QWidget *parent = nullptr);
  static Ctx *getContext();
  ~KodiSettings() override;
  Ui::KodiSettings *ui;

  QSharedPointer<KodiSrc> mdl;

signals:
  void autoCloseChatWindowsChanged(bool enabled);
  void inheritSystemThemeToggled(bool enabled);
  void enableDisplayGroupchatJoinLeaveToggled(bool enabled);
  void enterKeySendsChatToggled(bool enabled);
  void closing();
  void save(QSharedPointer<KodiSrc> ptr);
  void deleteKodiSrc(QSharedPointer<KodiSrc> ptr);

private slots:
  void onSave();
  void onTest();
  void onDelete();
  void onTestResult(bool status);

private:
  Ctx *m_ctx;
  static KodiSettings *pKodiSettings;
  void closeEvent(QCloseEvent *event) override;
  KodiSettingsForm formData();
};


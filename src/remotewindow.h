#pragma once

#include <iostream>

#include <QMainWindow>
#include <QCompleter>
#include <QPushButton>
#include <QClipboard>
#include <QStringListModel>
#include <QTimer>
#include <QQuickWidget>
#include <QQuickView>
#include <QQmlContext>
#include <QDebug>
#include <QQueue>
#include <QThread>
#include <QMutex>

#include "ctx.h"
#include "about.h"
#include "lib/utils.h"

namespace Ui {
  class RemoteWindow;
}

class Ctx;
class RemoteWindow : public QMainWindow
{
Q_OBJECT
Q_PROPERTY(bool inputBoxActive MEMBER inputBoxActive NOTIFY inputBoxActiveChanged);
Q_PROPERTY(bool windowActive MEMBER windowActive NOTIFY windowActiveChanged);

public:
  explicit RemoteWindow(Ctx *ctx, QSharedPointer<KodiSrc> mdl, QWidget *parent = nullptr);
  void setupUIModels();
  void queueTask();
  void setupCompleter();
  ~RemoteWindow();

  QSharedPointer<KodiSrc> mdl;

  bool windowActive = false;
  bool inputBoxActive = false;

signals:
  void inputBoxActiveChanged();
  void enterPressed();
  void windowActiveChanged();
  void closed(QSharedPointer<KodiSrc> ptr);

public slots:
  void onSetWindowTitle();

private slots:
  void onExample(int number);

protected:
  void closeEvent(QCloseEvent *event) override;
  bool eventFilter(QObject *watched, QEvent *event);
  void changeEvent(QEvent * e);


private:
  Ui::RemoteWindow *ui;
  Ctx *m_ctx = nullptr;

  QQuickWidget *m_quickWidget = nullptr;
  About *m_about = nullptr;


  void createQml();
  void destroyQml();
};

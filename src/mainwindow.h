#pragma once

#include <iostream>

#include <QMainWindow>
#include <QCompleter>
#include <QPushButton>
#include <QClipboard>
#include <QStringListModel>
#include <QTimer>
#include <QDebug>
#include <QQueue>
#include <QThread>
#include <QMutex>

#include "ctx.h"
#include "about.h"
#include "overview/overviewwidget.h"
#include "lib/utils.h"
#include "remotewindow.h"
#include "kodisettings.h"

namespace Ui {
  class MainWindow;
}

class Ctx;
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(Ctx *ctx, QWidget *parent = nullptr);
  void setupUIModels();
  void queueTask();
  void setupCompleter();
  ~MainWindow();

  qreal screenDpiRef;
  QRect screenGeo;
  QRect screenRect;
  qreal screenDpi;
  qreal screenDpiPhysical;
  qreal screenRatio;

public slots:
  void onOpenAboutWindow();
  void onQuitApplication();
  void onRemoteWindowClosed(QSharedPointer<KodiSrc> ptr);

private slots:
  void onNetworkScanStart();
  void onNetworkScanFinished();
  void onOverviewRowClicked(QSharedPointer<KodiSrc> ptr);
  void onOverviewSettingsClicked(QSharedPointer<KodiSrc> ptr);

private:
  Ui::MainWindow *ui;
  Ctx *m_ctx = nullptr;
  OverviewWidget *m_widgetOverview = nullptr;

  void btnScanSetText();
  QTimer *m_networkScanTimer;
  unsigned int m_networkScanSecs = 0;

  About *m_about = nullptr;
  KodiSettings *m_kodiSettings = nullptr;
  
  RemoteWindow* findWindow(QSharedPointer<KodiSrc> ptr);
  QList<RemoteWindow*> windows;
};

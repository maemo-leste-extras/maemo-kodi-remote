#include "remotewindow.h"
#include "ui_remotewindow.h"

RemoteWindow::RemoteWindow(Ctx *ctx, QSharedPointer<KodiSrc> mdl, QWidget *parent) :
    QMainWindow(parent),
    m_ctx(ctx),
    mdl(mdl),
    ui(new Ui::RemoteWindow)
{
  ui->setupUi(this);

  this->onSetWindowTitle();
  //this->resize(960, 540);
  this->show();
  this->createQml();
}

void RemoteWindow::createQml() {
  if(m_quickWidget != nullptr) return;
  m_quickWidget = new QQuickWidget(this);
  m_quickWidget->installEventFilter(this);
  m_quickWidget->setAttribute(Qt::WA_AlwaysStackOnTop);

  qmlRegisterUncreatableType<KodiSrc>("KodiSrc", 1, 0, "KodiSrc", "nope");
  qmlRegisterUncreatableType<KodiNowPlaying>("KodiNowPlaying", 1, 0, "KodiNowPlaying", "nope");
  qmlRegisterUncreatableType<KodiPlayer>("KodiNowPlaying", 1, 0, "KodiNowPlaying", "nope");
  
  auto *qctx = m_quickWidget->rootContext();
  qctx->setContextProperty("cfg", config());
  qctx->setContextProperty("ctx", m_ctx);
  qctx->setContextProperty("window", this);
  qctx->setContextProperty("mdl", mdl.get());
  qctx->setContextProperty("np", mdl->nowPlaying);
  qctx->setContextProperty("player", mdl->player);

  m_quickWidget->setSource(QUrl("qrc:/qml/main.qml"));
  m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

  // example signal from QML to C++
  connect((QObject*)m_quickWidget->rootObject(), SIGNAL(onExample(int)), this, SLOT(onExample(int)));

  connect(mdl.get(), &KodiSrc::statusChanged, [this](KodiStatus status){
    this->onSetWindowTitle();
  });

  ui->centralWidget->layout()->addWidget(m_quickWidget);
}

void RemoteWindow::destroyQml() {
  if(m_quickWidget == nullptr) return;
  m_quickWidget->disconnect();
  m_quickWidget->deleteLater();
  m_quickWidget = nullptr;
}

void RemoteWindow::onExample(int number) {
  qDebug() << "clicked: " + number;
}

void RemoteWindow::closeEvent(QCloseEvent *event) {
  emit closed(mdl);
  QWidget::closeEvent(event);
}

void RemoteWindow::onSetWindowTitle() {
  auto _title = mdl->name;
  if(mdl->status == KodiStatus::NO_ROUTE) 
    _title += " (unreachable)";
  else if(mdl->status == KodiStatus::NO_AUTH) 
    _title += " (unauthenticated)";
  this->setWindowTitle(_title);
}

void RemoteWindow::changeEvent(QEvent * e) {
    if(e->type() == QEvent::ActivationChange) {
      auto changed = windowActive != this->isActiveWindow();
      if(changed) {
        windowActive = this->isActiveWindow();
        emit windowActiveChanged();
        mdl->setWindowStatus(windowActive);
      }
    }
}

bool RemoteWindow::eventFilter(QObject *watched, QEvent *event) {
  switch(event->type()) {
    case QKeyEvent::KeyPress: {
      auto *ke = static_cast<QKeyEvent*>(event);
      auto key = ke->key();
      
      if(inputBoxActive && (key == Qt::Key_Return || key == Qt::Key_Enter)) {
        emit enterPressed();
      }

      if(!inputBoxActive) {
        // Input keys are sent as Kodi "actions" (Input.ExecuteAction) rather
        // than raw keys. Actions are routed to the currently focused Kodi
        // window, so they navigate the OSD (or lists) the way a keyboard
        // would, and -- unlike raw EventServer keys -- they are NOT remapped
        // by the user's own keyboard.xml.
        if(key == Qt::Key_Return || key == Qt::Key_Enter) {
          // keyboard-like Enter: opens the OSD if closed, otherwise activates
          // the focused OSD control (see KodiSrc::enterAction)
          mdl->enterAction();
        } else if(key == Qt::Key_H) {
          mdl->navAction("left");
        } else if(key == Qt::Key_J) {
          mdl->navAction("down");
        } else if(key == Qt::Key_K) {
          mdl->navAction("up");
        } else if(key == Qt::Key_L) {
          mdl->navAction("right");
        } else if(key == Qt::Key_0) {
          mdl->toggleMute();
        } else if(key == Qt::Key_1) {
          mdl->setVolume(10);
        } else if(key == Qt::Key_2) {
          mdl->setVolume(20);
        } else if(key == Qt::Key_3) {
          mdl->setVolume(30);
        } else if(key == Qt::Key_4) {
          mdl->setVolume(40);
        } else if(key == Qt::Key_5) {
          mdl->setVolume(50);
        } else if(key == Qt::Key_6) {
          mdl->setVolume(62);
        } else if(key == Qt::Key_7) {
          mdl->setVolume(70);
        } else if(key == Qt::Key_8) {
          mdl->setVolume(85);
        } else if(key == Qt::Key_9) {
          mdl->setVolume(100);
        } else if(key == Qt::Key_Backspace) {
          mdl->executeAction("back");
        } else if(key == Qt::Key_I) {
          mdl->executeAction("info");
        } else if(key == Qt::Key_O) {
          // player process info: codec / hardware decoding / resolution overlay
          // (what "o" does on a standard Kodi keyboard)
          mdl->executeAction("playerprocessinfo");
        } else if(key == Qt::Key_Tab) {
          // return to the playing fullscreen video (NOT togglefullscreen,
          // which prompts a resolution change)
          mdl->executeAction("fullscreen");
        } else if(key == Qt::Key_Up) {
          mdl->navAction("up");
        } else if(key == Qt::Key_Down) {
          mdl->navAction("down");
        } else if(key == Qt::Key_Left) {
          mdl->navAction("left");
        } else if(key == Qt::Key_Right) {
          mdl->navAction("right");
        } else if(key == Qt::Key_Space) {
          emit mdl->showKeyboard();
        }
      }
      break;
    }
  }

  return QMainWindow::eventFilter(watched, event);
}

RemoteWindow::~RemoteWindow() {
  delete ui;
}

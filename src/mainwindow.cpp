#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(Ctx *ctx, QWidget *parent) :
    QMainWindow(parent),
    m_ctx(ctx),
    m_networkScanTimer(new QTimer(this)),
    ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  this->screenDpiRef = 128;
  this->screenGeo = QApplication::primaryScreen()->availableGeometry();
  this->screenRect = QGuiApplication::primaryScreen()->geometry();
  this->screenDpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
  this->screenDpiPhysical = QGuiApplication::primaryScreen()->physicalDotsPerInch();
  this->screenRatio = this->screenDpiPhysical / this->screenDpiRef;
  qDebug() << QString("%1x%2 (%3 DPI)").arg(
      this->screenRect.width()).arg(this->screenRect.height()).arg(this->screenDpi);

  m_widgetOverview = new OverviewWidget(ctx, this);
  ui->overviewLayout->addWidget(m_widgetOverview);

  unsigned int show_page = m_ctx->overviewModel->sources.length() == 0 ? 1 : 0;
  ui->stackedWidget->setCurrentIndex(show_page);

  // @TODO: implement virtual keyboard
  ui->label_3->hide();
  ui->label_4->hide();
  ui->check_keyboard->hide();
  this->show();

  connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onOpenAboutWindow);
  connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onQuitApplication);

  connect(m_widgetOverview, &OverviewWidget::overviewRowClicked, this, &MainWindow::onOverviewRowClicked);
  connect(m_widgetOverview, &OverviewWidget::overviewSettingsClicked, this, &MainWindow::onOverviewSettingsClicked);

  m_networkScanTimer->setInterval(1000);
  connect(m_networkScanTimer, &QTimer::timeout, [this] {
    m_networkScanSecs += 1;
    this->btnScanSetText();
  });
  connect(m_ctx->network, &Network::networkScanStart, this, &MainWindow::onNetworkScanStart);
  connect(m_ctx->network, &Network::networkScanFinished, this, &MainWindow::onNetworkScanFinished);

  // welcome page
  connect(ui->btn_continue, &QPushButton::clicked, [this] {
    auto physical_keyboard_enabled = ui->check_keyboard->checkState() == Qt::Checked;
    config()->set(ConfigKeys::HasKeyboard, physical_keyboard_enabled);
    ui->stackedWidget->setCurrentIndex(0);
  });


  // overview page
  connect(ui->btn_addKodiSrc, &QPushButton::clicked, [this] {
    this->onOverviewSettingsClicked(nullptr);
  });

  connect(ui->btn_scan, &QPushButton::clicked, [this] {
    if(!m_ctx->network->isScanning) {
      m_ctx->network->onScanForKodiInstances();
      this->btnScanSetText();
    } else {
      ui->btn_scan->setEnabled(true);
      this->btnScanSetText();
    }
  });

  this->setWindowTitle("maemo-kodi-remote");
}

void MainWindow::onOverviewRowClicked(QSharedPointer<KodiSrc> ptr) {
  RemoteWindow *win = this->findWindow(ptr);
  if(win != nullptr) {
    win->raise();
    return;
  }

  win = new RemoteWindow(m_ctx, ptr, this);
  windows << win;
  connect(win, &RemoteWindow::closed, this, &MainWindow::onRemoteWindowClosed);
  connect(ptr.data(), &KodiSrc::nameChanged, win, &RemoteWindow::onSetWindowTitle);
}

void MainWindow::onRemoteWindowClosed(QSharedPointer<KodiSrc> ptr) {
  auto *win = this->findWindow(ptr);
  if(win != nullptr) {
    windows.removeAll(win);
    win->deleteLater();
  }
}

void MainWindow::onOverviewSettingsClicked(QSharedPointer<KodiSrc> ptr) {
  if(m_kodiSettings != nullptr) 
    return;

  m_kodiSettings = new KodiSettings(m_ctx, ptr, this);
  connect(m_kodiSettings, &KodiSettings::save, [this](QSharedPointer<KodiSrc> ptr) {
    m_ctx->overviewModel->addItem(ptr);
    m_ctx->overviewModel->saveToCfg();
  });

  connect(m_kodiSettings, &KodiSettings::deleteKodiSrc, [this](QSharedPointer<KodiSrc> ptr) {
    // close associated window
    auto *win = this->findWindow(ptr);
    if(win != nullptr) {
      win->close();
    }

    m_ctx->overviewModel->deleteItem(ptr);
    m_ctx->overviewModel->saveToCfg();
    //ptr->deleteLater();
  });

  connect(m_kodiSettings, &KodiSettings::closing, [this] {
    m_kodiSettings = nullptr;
  });

  m_kodiSettings->show();
}

void MainWindow::onNetworkScanStart() {
  m_networkScanTimer->start();
}

void MainWindow::onNetworkScanFinished() {
  m_networkScanTimer->stop();
  this->btnScanSetText();
}

void MainWindow::btnScanSetText() {
  if(!m_ctx->network->isScanning) {
    ui->btn_scan->setText("Auto-Discovery");
    ui->btn_scan->setEnabled(true);
    this->setWindowTitle("maemo-kodi-remote");
  } else {
    QString msg = QString("Scanning (%1s/10s)").arg(m_networkScanSecs);
    this->setWindowTitle(msg);
    ui->btn_scan->setText(msg);
    ui->btn_scan->setEnabled(false);
  }
}

RemoteWindow* MainWindow::findWindow(QSharedPointer<KodiSrc> ptr) {
  for(auto *win: windows) {
    if(win != nullptr && win->mdl == ptr)
      return win;
  }
  return nullptr;
}

void MainWindow::onOpenAboutWindow() {
  m_about = new About(m_ctx, this);
  m_about->show();
}

void MainWindow::onQuitApplication() {
  this->close();
}

MainWindow::~MainWindow() {
  delete ui;
}


#include <QPixmap>
#include <QMessageBox>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QSystemTrayIcon>
#include <QQmlContext>
#include <QMessageBox>
#include <QGroupBox>
#include <QFileDialog>
#include <QCheckBox>

#include "kodisettings.h"

#include "ui_kodisettings.h"

KodiSettings * KodiSettings::pKodiSettings = nullptr;

KodiSettings::KodiSettings(Ctx *ctx, QSharedPointer<KodiSrc> mdl, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::KodiSettings),
    mdl(mdl),
    m_ctx(ctx) {
  pKodiSettings = this;
  ui->setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);

  setProperty("X-Maemo-StackedWindow", 1);

  if(mdl != nullptr) {
    this->setWindowTitle(QString("Edit: %1").arg(mdl->name));
    ui->line_name->setText(mdl->name);
    ui->line_ip->setText(mdl->address);
    ui->line_port->setText(QString::number(mdl->port));
    ui->line_username->setText(mdl->username);
    ui->line_password->setText(mdl->password);
    ui->combo_scheme->setCurrentIndex(mdl->scheme == "http" ? 0 : 1);
    connect(ui->btn_delete, &QPushButton::clicked, this, &KodiSettings::onDelete);
  } else {
    ui->btn_delete->hide();
    this->setWindowTitle(QString("Add: new server"));
  }

  connect(m_ctx->network, &Network::simplePing, this, &KodiSettings::onTestResult);
  connect(m_ctx->network, &Network::simplePingError, [this](QProcess::ProcessError error){
    QString msg = "onSimplePingError";
    Utils::messageBox(msg);
    ui->btn_test->setEnabled(true);
  });

  connect(ui->btn_test, &QPushButton::clicked, this, &KodiSettings::onTest);
  connect(ui->btn_save, &QPushButton::clicked, this, &KodiSettings::onSave);
}

void KodiSettings::onDelete() {
  emit deleteKodiSrc(mdl);
  this->close();
}

void KodiSettings::onSave() {
  auto form = formData();
  if(!form.valid()) {
    qWarning() << form.errorMsg;
    Utils::messageBox(form.errorMsg);
    return;
  }

  if(mdl == nullptr) {
    // const QString &name, const QString &address, unsigned int port, const QString &username, const QString &password, QObject *parent = nullptr
    mdl = QSharedPointer<KodiSrc>(new KodiSrc(form.name, form.address, form.scheme, form.port, form.username, form.password));
    mdl->name = form.name;
    mdl->address = form.address;
    mdl->port = form.port;
    mdl->scheme = form.scheme;
    mdl->username = form.username;
    mdl->password = form.password;
  } else {
    mdl->setName(form.name);
    mdl->setAddress(form.address);
    mdl->setPort(form.port);
    mdl->setScheme(form.scheme);
    mdl->setCredentials(form.username, form.password);
    emit mdl->kodiURLChanged(mdl->urlRPC(), form.username, form.password);
  }

  emit save(mdl);
  this->close();
}

void KodiSettings::onTestResult(bool status) {
  ui->btn_test->setEnabled(true);
  if(!status) {
    QString msg = "Port was open, but could not communicate with Kodi. Wrong credentials maybe?";
    qWarning() << "status";
    Utils::messageBox(msg);
    return;
  }

  Utils::messageBox("Kodi was successfully contacted.");
}

void KodiSettings::onTest() {
  ui->btn_test->setEnabled(false);
  auto form = formData();

  if(!form.valid()) {
    qWarning() << form.errorMsg;
    Utils::messageBox(form.errorMsg);
    ui->btn_test->setEnabled(true);
    return;
  }

  if(!Utils::portOpen(form.address, form.port)) {
    auto msg = QString("port %1 on host %2 is not opened").arg(QString::number(form.port), form.address);
    qWarning() << msg;
    Utils::messageBox(msg);
    ui->btn_test->setEnabled(true);
    return;
  }

  m_ctx->network->onSimplePing(
    form.address, 
    form.port, 
    form.scheme == "https",
    form.username,
    form.password);
}

KodiSettingsForm KodiSettings::formData() {
  auto name = ui->line_name->text();
  auto ip = ui->line_ip->text();
  unsigned int port = ui->line_port->text().toInt();
  auto username = ui->line_username->text();
  auto password = ui->line_password->text();
  auto scheme = ui->combo_scheme->currentText();
  KodiSettingsForm settings;
  settings.name = name;
  settings.address = ip;
  settings.port = port;
  settings.username = username;
  settings.password = password;
  settings.scheme = scheme;
  return settings;
}

Ctx *KodiSettings::getContext(){
  return pKodiSettings->m_ctx;
}

void KodiSettings::closeEvent(QCloseEvent *event) {
  emit closing();
  QWidget::closeEvent(event);
}

KodiSettings::~KodiSettings() {
  qDebug() << "~KodiSettings";
  delete ui;
}

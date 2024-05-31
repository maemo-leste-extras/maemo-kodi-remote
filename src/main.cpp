#include <QApplication>
#include <QResource>
#include <QtCore>
#include <QSslSocket>

#include <unistd.h>
#include <sys/types.h>
#include "mainwindow.h"
#include "lib/logger.h"
#include "ctx.h"

int main(int argc, char *argv[]) {
  Q_INIT_RESOURCE(assets);

  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication::setApplicationName("maemo-kodi-remote");
  QApplication::setOrganizationDomain("maemo-leste.github.io");
  QApplication::setOrganizationName("Maemo Leste");
  QApplication app(argc, argv);
  qInstallMessageHandler(appMessageHandler);

  auto *ctx = new Ctx();
  logger_ctx = ctx;
  ctx->isDebug = false;
  auto *mainWindow = new MainWindow(ctx);

  return QApplication::exec();
}

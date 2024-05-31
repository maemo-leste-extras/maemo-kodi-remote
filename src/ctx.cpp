#include <filesystem>

#include <QObject>
#include <QDir>
#include <QStandardPaths>

#include "ctx.h"
#include "lib/utils.h"

using namespace std::chrono;

Ctx::Ctx(QObject *parent) : QObject(parent) {
  configRoot = QDir::homePath();
  homeDir = QDir::homePath();
  configDirectory = QString("%1/.config/%2/").arg(configRoot, QCoreApplication::applicationName());
  createConfigDirectory(configDirectory);
  unpackAppArtifacts();

  network = new Network(configDirectory, this);
  overviewModel = new OverviewModel(this);  
  connect(network, &Network::networkScanResult, overviewModel, &OverviewModel::onNetworkScanResult);
}

void Ctx::createConfigDirectory(const QString &dir) {
  QStringList createDirs({dir});
  for(const auto &d: createDirs) {
    if(!std::filesystem::exists(d.toStdString())) {
      qDebug() << QString("Creating directory: %1").arg(d);
      if (!QDir().mkpath(d))
        throw std::runtime_error("Could not create directory " + d.toStdString());
    }
  }
}

void Ctx::unpackAppArtifacts() {
  auto dest = configDirectory;
  auto files = {
      ":/assets/kodiscan.sh",
      ":/assets/kodiping.sh"
  };

  for(const auto &fp: files) {
    if(!Utils::fileExists(fp)) {
      qWarning() << fp << "does not exist";
      continue;
    }

    QFile f(fp);
    QFileInfo fileInfo(f);

    auto to_path = QString("%1/%2").arg(dest, fileInfo.fileName());
    if(Utils::fileExists(to_path)) {
      QFile file(to_path);
      file.remove();
    }

    qDebug() << "writing" << to_path;
    f.copy(to_path);
    f.setPermissions(QFile::ExeUser);
    f.close();
  }
}

void Ctx::singleShot(int msec, QJSValue callback) {
  QTimer::singleShot(msec, this, [callback] () mutable {
    if (callback.isCallable())
      callback.call();
  });
}

void Ctx::applicationLog(const QString &msg) {

}

Ctx::~Ctx() {}

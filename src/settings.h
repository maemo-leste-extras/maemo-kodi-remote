#pragma once
#include <QtGlobal>
#include <QResource>
#include <QApplication>
#include <QScreen>
#include <QtWidgets/QMenu>
#include <QMainWindow>
#include <QObject>
#include <QtCore>
#include <QtGui>
#include <QFileInfo>

#include <iostream>

#include "ctx.h"
#include "lib/config.h"

namespace Ui {
    class Settings;
}

class Settings : public QMainWindow {
    Q_OBJECT

public:
    explicit Settings(Ctx *ctx, QWidget *parent = nullptr);
    static Ctx *getContext();
    ~Settings() override;
    Ui::Settings *ui;

signals:
    void textScalingChanged();
    void autoCloseChatWindowsChanged(bool enabled);
    void inheritSystemThemeToggled(bool enabled);
    void enableDisplayGroupchatJoinLeaveToggled(bool enabled);
    void enterKeySendsChatToggled(bool enabled);

private slots:
    void onTextScalingValueChanged(int val);

private:
    Ctx *m_ctx;
    static Settings *pSettings;
    void closeEvent(QCloseEvent *event) override;
};


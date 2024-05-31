#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

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
    class About;
}

class About : public QMainWindow {
    Q_OBJECT

public:
    explicit About(Ctx *ctx, QWidget *parent = nullptr);
    static Ctx *getContext();
    ~About() override;
    Ui::About *ui;


private:
    Ctx *m_ctx;
    static About *pAbout;
    void closeEvent(QCloseEvent *event) override;
};

#endif

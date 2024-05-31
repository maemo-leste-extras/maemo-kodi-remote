#pragma once
#include <QCompleter>
#include <QCheckBox>
#include <QPushButton>
#include <QStringList>
#include <QClipboard>
#include <QTableWidget>
#include <QScroller>
#include <QStringListModel>
#include <QTimer>
#include <QEasingCurve>
#include <QMessageBox>
#include <QWidget>
#include <QMenu>

#include "ctx.h"
#include "lib/utils.h"
#include "overview/OverviewModel.h"

namespace Ui {
  class OverviewWidget;
}

class OverviewWidget : public QWidget
{
Q_OBJECT

public:
  explicit OverviewWidget(Ctx *ctx, QWidget *parent = nullptr);
  ~OverviewWidget() override;

signals:
  void overviewRowClicked(QSharedPointer<KodiSrc> ptr);
  void overviewSettingsClicked(QSharedPointer<KodiSrc> ptr);

private:
  void setupUITable();

  Ui::OverviewWidget *ui;
  Ctx *m_ctx;
};

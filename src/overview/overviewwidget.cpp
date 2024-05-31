#include "overview/overviewwidget.h"
#include "overview/ui_overviewwidget.h"

OverviewWidget::OverviewWidget(Ctx *ctx, QWidget *parent) :
    QWidget(parent),
    m_ctx(ctx),
    ui(new Ui::OverviewWidget)
{
  ui->setupUi(this);
  this->setupUITable();
}

void OverviewWidget::setupUITable() {
  auto *table = ui->tableOverview;

  // enable kinetic scrolling, disable overshoot
  QScroller::grabGesture(table, QScroller::LeftMouseButtonGesture);
  QScroller *scroller = QScroller::scroller(table);
  QScrollerProperties properties = QScroller::scroller(scroller)->scrollerProperties();
  QVariant overshootPolicy = QVariant::fromValue<QScrollerProperties::OvershootPolicy>(QScrollerProperties::OvershootAlwaysOff);
  properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, overshootPolicy);
  scroller->setScrollerProperties(properties);
  table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // row height
  const auto header = table->horizontalHeader();
  QHeaderView *verticalHeader = table->verticalHeader();
  verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
  verticalHeader->setDefaultSectionSize(m_ctx->overviewModel->itemHeight);

  table->setModel(m_ctx->overviewModel);

  auto richItemDelegate = new RichItemDelegate(this);
  richItemDelegate->setStyleSheet(QString(Utils::fileOpen(":/overviewRichDelegate.css")));
  table->setItemDelegateForColumn(OverviewModel::ContentRole, richItemDelegate);

  header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  header->setSectionResizeMode(1, QHeaderView::Stretch);
  header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
  table->setFocusPolicy(Qt::NoFocus);

  // click handler
  connect(table, &QAbstractItemView::clicked, this, [this](const QModelIndex& idx) {
    unsigned int col = idx.column();
    unsigned int row = idx.row();
    QSharedPointer<KodiSrc> item = m_ctx->overviewModel->sources.at(row);

    if(col == 2) {  // settings
      emit overviewSettingsClicked(item);
    } else {
      emit overviewRowClicked(item);
    }
  });
}

// (int row, int column)
OverviewWidget::~OverviewWidget() {
  delete ui;
}

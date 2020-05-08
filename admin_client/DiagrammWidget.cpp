#include "DiagrammWidget.hpp"

#include <QCalendarWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

#include <QtCore/QAbstractTableModel>

#include "Charts.hpp"

DiagrammWidget::DiagrammWidget(QWidget *parent) : QWidget(parent) {

    auto layout = new QVBoxLayout;
    auto label = new QLabel(tr("If you wanto to make a diagramm, pls select first of all a begin and end date"));

    auto calendarLayout = new QVBoxLayout;
    auto now = QDateTime::currentDateTime();

    auto *beginDate = new QCalendarWidget;
    beginDate->setGridVisible(true);
    // beginDate->setMinimumDate(QDate(2012, 12, 12));
    // beginDate->setMaximumDate(QDate(now.date().year, now.date().month, now.date().day));

    auto *endDate = new QCalendarWidget;
    endDate->setGridVisible(true);
    //  beginDate->setMinimumDate(QDate(2012, 12, 12));
    // endDate->setMaximumDate(QDate(now.date().year, now.date().month, now.date().day));

    calendarLayout->addWidget(beginDate);
    calendarLayout->addWidget(endDate);

    auto buttonLayout = new QHBoxLayout;

    auto incomeButton = new QPushButton(tr("Income / day"));
    auto avgFoodOrderButton = new QPushButton(tr("Avarage food and order / day"));
    auto avgFoodCountOrderButton = new QPushButton(tr("Avarage food count and order / day"));
    auto orderDayButton = new QPushButton(tr(" Orders / day"));

    buttonLayout->addWidget(incomeButton);
    buttonLayout->addWidget(avgFoodOrderButton);
    buttonLayout->addWidget(avgFoodCountOrderButton);
    buttonLayout->addWidget(orderDayButton);

    layout->addWidget(label);
    layout->addLayout(calendarLayout, 1);
    layout->addLayout(buttonLayout, 1);

    setLayout(layout);

    connect(incomeButton, &QPushButton::clicked, this,
            [this, beginDate, endDate] { openChartDialog(ChartKind::Income, beginDate->selectedDate(), endDate->selectedDate()); });

    connect(avgFoodOrderButton, &QPushButton::clicked, this, [this, beginDate, endDate] {
        openChartDialog(ChartKind::FoodKindAverage, beginDate->selectedDate(), endDate->selectedDate());
    });

    connect(avgFoodCountOrderButton, &QPushButton::clicked, this, [this, beginDate, endDate] {
        openChartDialog(ChartKind::FoodAmountAverage, beginDate->selectedDate(), endDate->selectedDate());
    });

    connect(orderDayButton, &QPushButton::clicked, this, [this, beginDate, endDate] {
        openChartDialog(ChartKind::OrderCount, beginDate->selectedDate(), endDate->selectedDate());
    });
}

void DiagrammWidget::refreshData(const std::vector<Orders> &value) {
    if (activeDialog_ == nullptr) {
        return;
    }
    std::vector<Orders> filtered;
    std::uint64_t beginTimestamp = QDateTime(chartBegin_).toSecsSinceEpoch();
    std::uint64_t endTimestamp = QDateTime(chartEnd_.addDays(1)).toSecsSinceEpoch();
    std::copy_if(value.begin(), value.end(), std::back_inserter(filtered),
                 [beginTimestamp, endTimestamp](const Orders &order) {
                     return beginTimestamp <= order.OrderedDate && order.OrderedDate <= endTimestamp;
                 });
    activeDialog_->setChart(createChart(chartKind_, filtered));
}

void DiagrammWidget::openChartDialog(ChartKind kind, const QDate& beginDate, const QDate& endDate) {
    activeDialog_ = new ChartDialog(this);

    chartKind_ = kind;
    chartBegin_ = beginDate;
    chartEnd_ = endDate;
    activeDialog_->setChart(createChart(chartKind_, {}));

    dataRequest(chartBegin_, chartEnd_);

    activeDialog_->exec();
    activeDialog_ = nullptr;
}

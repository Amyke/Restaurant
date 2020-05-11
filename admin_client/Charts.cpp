#include "Charts.hpp"

#include <numeric>

#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLineSeries>

#include <QtCharts/QValueAxis>

#include <QtCore/QIdentityProxyModel>
#include <QtWidgets/QHBoxLayout>

#include "OrdersModel.hpp"

ChartDialog::ChartDialog(QWidget *parent) : QDialog(parent) {
    setMinimumSize(640, 480);

    view_ = new QtCharts::QChartView;
    view_->setRenderHint(QPainter::Antialiasing);

    auto layout = new QHBoxLayout;
    layout->addWidget(view_);
    setLayout(layout);
}

void ChartDialog::setChart(QtCharts::QChart *chart) {
    view_->setChart(chart);
}

QtCharts::QChart *createIncomeChart(const std::vector<Orders> &orders);
QtCharts::QChart *createFoodKindAverageChart(const std::vector<Orders> &orders);
QtCharts::QChart *createFoodAmountAverageChart(const std::vector<Orders> &orders);
QtCharts::QChart *createOrderCountChart(const std::vector<Orders> &orders);

QtCharts::QChart *createChart(ChartKind kind, const std::vector<Orders> &orders) {
    switch (kind) {
    case ChartKind::Income:
        return createIncomeChart(orders);
    case ChartKind::FoodKindAverage:
        return createFoodKindAverageChart(orders);
    case ChartKind::FoodAmountAverage:
        return createFoodAmountAverageChart(orders);
    case ChartKind::OrderCount:
        return createOrderCountChart(orders);
    }
    return nullptr;
}

QtCharts::QChart *createIncomeChart(const std::vector<Orders> &orders) {
    std::map<QDate, int> data;
    for (const auto &order : orders) {
        if (order.Status != OrderStatus::Payed) {
            continue;
        }
        QDate date = QDateTime::fromTime_t(order.OrderedDate).date();
        data[date] +=
            std::accumulate(order.OrderedFoods.begin(), order.OrderedFoods.end(), 0,
                            [](int acc, const FoodContains &food) { return acc + food.Amount * food.FoodPrice; });
    }

    auto series = new QtCharts::QLineSeries;
    int max = 0;
    for (const auto &[date, income] : data) {
        series->append(QDateTime(date).toMSecsSinceEpoch(), income);
        max = income > max ? income : max;
    }

    // series->append(QDateTime(QDate(2020, 05, 07)).toMSecsSinceEpoch(), 5);
    // series->append(QDateTime(QDate::currentDate()).toMSecsSinceEpoch(), 10);
    // max = 10;

    auto theme = QtCharts::QChart::ChartTheme::ChartThemeBlueCerulean;

    auto chart = new QtCharts::QChart;
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle(ChartDialog::tr("Income / day"));
    chart->setTheme(theme);

    auto axisX = new QtCharts::QDateTimeAxis;
    axisX->setTickCount(std::min(static_cast<std::size_t>(2), data.size()));
    axisX->setFormat(ChartDialog::tr("yyyy.MM.dd"));
    axisX->setTitleText(ChartDialog::tr("Date"));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto axisY = new QtCharts::QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTitleText(ChartDialog::tr("Money"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    axisY->setRange(0, max <= 0 ? 1 : max);

    return chart;
}

QtCharts::QChart *createFoodKindAverageChart(const std::vector<Orders> &orders) {

    // date -> (count(food kinds), count(orders))
    std::map<QDate, std::pair<int, int>> data;
    for (const auto &order : orders) {
        if (order.Status != OrderStatus::Payed) {
            continue;
        }
        QDate date = QDateTime::fromTime_t(order.OrderedDate).date();
        auto it = data.try_emplace(date, std::make_pair(0, 0)).first;
        it->second.first = std::accumulate(order.OrderedFoods.begin(), order.OrderedFoods.end(), it->second.first,
                                           [](int acc, const FoodContains &food) { return acc + 1; });
        it->second.second += 1;
    }

    auto series = new QtCharts::QBarSeries;

    auto axisX = new QtCharts::QBarCategoryAxis;

    auto bar = new QtCharts::QBarSet("Food kind / order");

    double max = 0.0;
    for (const auto &[date, kindsAndOrders] : data) {
        double average = static_cast<double>(kindsAndOrders.first) / kindsAndOrders.second;
        bar->append(average);
        axisX->append(date.toString(ChartDialog::tr("yyyy.MM.dd")));
        max = average > max ? average : max;
    }

    series->append(bar);

    auto theme = QtCharts::QChart::ChartTheme::ChartThemeBlueCerulean;
    auto chart = new QtCharts::QChart;
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle(ChartDialog::tr("Avarage food count / order"));
    chart->setTheme(theme);

    axisX->setTitleText(ChartDialog::tr("Date"));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto axisY = new QtCharts::QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTitleText(ChartDialog::tr("Avarage Food/Count"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    axisY->setRange(0, max <= 0 ? 1 : max);

    return chart;
}

QtCharts::QChart *createFoodAmountAverageChart(const std::vector<Orders> &orders) {

    // date -> (count(food kinds), count(orders))
    std::map<QDate, std::pair<int, int>> data;
    for (const auto &order : orders) {
        if (order.Status != OrderStatus::Payed) {
            continue;
        }
        QDate date = QDateTime::fromTime_t(order.OrderedDate).date();
        auto it = data.try_emplace(date, std::make_pair(0, 0)).first;
        it->second.first = std::accumulate(order.OrderedFoods.begin(), order.OrderedFoods.end(), it->second.first,
                                           [](int acc, const FoodContains &food) { return acc + food.Amount; });
        it->second.second += 1;
    }

    auto series = new QtCharts::QBarSeries;

    auto axisX = new QtCharts::QBarCategoryAxis;

    auto bar = new QtCharts::QBarSet("Food count / order");

    double max = 0.0;
    for (const auto &[date, kindsAndOrders] : data) {
        double average = static_cast<double>(kindsAndOrders.first) / kindsAndOrders.second;
        bar->append(average);
        axisX->append(date.toString(ChartDialog::tr("yyyy.MM.dd")));
        max = average > max ? average : max;
    }

    series->append(bar);
    auto theme = QtCharts::QChart::ChartTheme::ChartThemeBlueCerulean;
    auto chart = new QtCharts::QChart;
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle(ChartDialog::tr("Avarage food count / order"));
    chart->setTheme(theme);

    axisX->setTitleText(ChartDialog::tr("Date"));

    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto axisY = new QtCharts::QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTitleText(ChartDialog::tr("Avarage Food/Count"));
    series->attachAxis(axisY);
    axisY->setRange(0, max <= 0 ? 1 : max);

    chart->addAxis(axisY, Qt::AlignLeft);

    return chart;
}

QtCharts::QChart *createOrderCountChart(const std::vector<Orders> &orders) {

    std::map<QDate, int> data;
    for (const auto &order : orders) {
        if (order.Status != OrderStatus::Payed) {
            continue;
        }
        QDate date = QDateTime::fromTime_t(order.OrderedDate).date();
        data[date] += 1;
    }

    auto series = new QtCharts::QLineSeries;
    int max = 0;
    for (const auto &[date, income] : data) {
        series->append(QDateTime(date).toMSecsSinceEpoch(), income);
        max = income > max ? income : max;
    }

    // series->append(QDateTime(QDate(2020, 05, 07)).toMSecsSinceEpoch(), 7);
    // series->append(QDateTime(QDate::currentDate()).toMSecsSinceEpoch(), 16);
    // max = 20;

    auto theme = QtCharts::QChart::ChartTheme::ChartThemeBlueCerulean;

    auto chart = new QtCharts::QChart;
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle(ChartDialog::tr("Orders / day"));
    chart->setTheme(theme);

    auto axisX = new QtCharts::QDateTimeAxis;
    axisX->setTickCount(std::min(static_cast<std::size_t>(2), data.size()));
    axisX->setFormat(ChartDialog::tr("yyyy.MM.dd"));
    axisX->setTitleText(ChartDialog::tr("Date"));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto axisY = new QtCharts::QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTitleText(ChartDialog::tr("Sum Orders"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    axisY->setRange(0, max <= 0 ? 1 : max);

    return chart;
}

#ifndef CHARTS_HPP
#define CHARTS_HPP

#include <map>

#include <QtCore/QDate>
#include <QtWidgets/QDialog>

#include "Message.hpp"

namespace QtCharts {
class QChart;
class QChartView;
} // namespace QtCharts

enum class ChartKind { Income, FoodKindAverage, OrderCount, FoodAmountAverage };

class ChartDialog : public QDialog {
    Q_OBJECT
public:
    explicit ChartDialog(QWidget *parent);

    void setChart(QtCharts::QChart *chart);

private:
    QtCharts::QChartView *view_;
};

QtCharts::QChart *createChart(ChartKind, const std::vector<Orders> &orders);

#endif //! CHARTS_HPP
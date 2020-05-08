#ifndef DIAGRAMMWIDGET_HPP
#define DIAGRAMMWIDGET_HPP

#include <QtCore/QDate>
#include <QtWidgets/QStackedWidget>

#include "Message.hpp"

class ChartDialog;
enum class ChartKind;

class DiagrammWidget : public QWidget {
    Q_OBJECT
public:
    explicit DiagrammWidget(QWidget *parent = nullptr);

    void refreshData(const std::vector<Orders> &value);

signals:
    void dataRequest(const QDate &begin, const QDate &end);

private:
    void openChartDialog(ChartKind, const QDate& begin, const QDate& end);

    ChartDialog *activeDialog_ = nullptr;
    ChartKind chartKind_;
    QDate chartBegin_;
    QDate chartEnd_;
};

#endif //! DIAGRAMMWIDGET_HPP
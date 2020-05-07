#ifndef ORDERDETAILSWIDGET_HPP
#define ORDERDETAILSWIDGET_HPP

#include <QtWidgets/QStackedWidget>

#include "Message.hpp"

class RequestedFoodModel;
class QDataWidgetMapper;
class QPushButton;

class OrderDetailsWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(std::vector<FoodContains> foodList READ foodList WRITE setFoodList)
public:
    explicit OrderDetailsWidget(QDataWidgetMapper *mapper, QWidget *parent = nullptr);

    std::vector<FoodContains> foodList() const;
    void setFoodList(const std::vector<FoodContains> &);

    void orderChanged();

signals:
    void inProgress();
    void completed();
    void payed();

private:
    QDataWidgetMapper *mapper_;
    RequestedFoodModel *orders_;

    QPushButton *inProgressButton_;
    QPushButton *completedButton_;
    QPushButton *payedButton_;
};

#endif //! ORDERDETAILSWIDGET_HPP
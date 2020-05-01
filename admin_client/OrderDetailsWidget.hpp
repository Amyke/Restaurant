#ifndef ORDERDETAILSWIDGET_HPP
#define ORDERDETAILSWIDGET_HPP

#include <QtWidgets/QStackedWidget>
#include "Message.hpp"

class RequestedFoodModel;
class QDataWidgetMapper;

class OrderDetailsWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(std::vector<FoodContains> foodList READ foodList WRITE setFoodList)
public:
    explicit OrderDetailsWidget(QDataWidgetMapper *mapper, QWidget *parent = nullptr);

    std::vector<FoodContains> foodList() const;
    void setFoodList(const std::vector<FoodContains> &);

signals:
    void inProgress();
    void complated();
    void payed();

private:
    RequestedFoodModel *orders_;
};

#endif //! ORDERDETAILSWIDGET_HPP
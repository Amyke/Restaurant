#ifndef ORDERWIDGET_HPP
#define ORDERWIDGET_HPP

#include <QtWidgets/QStackedWidget>
#include <vector>

#include "Message.hpp"

class MenuModel;
class CartModel;

class OrderWidget : public QWidget {
    Q_OBJECT
public:
    explicit OrderWidget(QWidget *parent = nullptr);

    void setFoodList(const std::vector<FoodContains> &foodList);
    void resetCart();

signals:
    void orderRequested(const std::vector<FoodAmount> &ordered);

private:
    MenuModel *foodListModel_;
    CartModel* cartModel_;
    std::vector<FoodAmount> orderRequestFoods;
};

#endif //! ORDERWIDGET_HPP
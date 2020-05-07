#ifndef PAYWIDGET_HPP
#define PAYWIDGET_HPP

#include <QtWidgets/QStackedWidget>

#include "Message.hpp"

class OrderModel;
class QPushButton;
class QLabel;

class PayWidget : public QWidget {
    Q_OBJECT;

public:
    explicit PayWidget(QWidget *parent = nullptr);

    void setFoodList(const std::vector<FoodContains> &foodList);

    void payFailed();

    void setOrderStatus(OrderStatus status);

signals:
    void payed();

private:
    OrderModel *model_;
    OrderStatus currentStatus_;

    QLabel *statusLabel_;
    QPushButton *payButton_;
    QLabel *allPrice_;
};

#endif //! PAYWIDGET_HPP

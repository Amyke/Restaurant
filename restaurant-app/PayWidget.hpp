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

signals:
    void payed();

private:
    QLabel *allPrice_;

    OrderModel *model_;
    QPushButton *payButton_;
};

#endif //! PAYWIDGET_HPP

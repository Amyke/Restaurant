#include "PayWidget.hpp"
#include "OrderModel.hpp"

#include <QtCore/QString>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>

#include <QtCore/QAbstractTableModel>

#include "Model.hpp"

PayWidget::PayWidget(QWidget *parent) : QWidget(parent) {

    auto layout = new QVBoxLayout;

    statusLabel_ = new QLabel;

    auto selectFood = new QTableView;
    model_ = new OrderModel(selectFood);
    selectFood->setModel(model_);
    selectFood->setStyleSheet(" QTableViewEdit {border-style: outset}");

    payButton_ = new QPushButton(tr("Pay"));
    allPrice_ = new QLabel(tr("Price: "));

    layout->addWidget(statusLabel_);
    layout->addWidget(selectFood);
    layout->addWidget(payButton_);
    layout->addWidget(allPrice_);

    setLayout(layout);

    connect(payButton_, &QPushButton::clicked, this, [this] {
        payed();

        payButton_->setEnabled(false);
    });
}

void PayWidget::setFoodList(const std::vector<FoodContains> &foodList) {
    model_->setFoodList(foodList);
    int price = std::accumulate(foodList.begin(), foodList.end(), 0, [](int sum, const FoodContains &value) {
        return sum + value.FoodPrice * value.Amount;
    });
    allPrice_->setText(tr("Price: %1").arg(price));
    setOrderStatus(OrderStatus::Pending);
}

void PayWidget::payFailed() {
    payButton_->setEnabled(currentStatus_ == OrderStatus::Completed);
}

void PayWidget::setOrderStatus(OrderStatus status) {
    payButton_->setEnabled(status == OrderStatus::Completed);
    currentStatus_ = status;
    switch (status) {
    case OrderStatus::Pending:
        statusLabel_->setText(tr("Your order is being processed..."));
        break;
    case OrderStatus::InProgress:
        statusLabel_->setText(tr("Your order has been processed!"));
        break;
    case OrderStatus::Completed:
        statusLabel_->setText(tr("Bon appetite!"));
        break;
    case OrderStatus::PayIntent:
        statusLabel_->setText(tr("The waiter was notified about your intention"));
        break;
    case OrderStatus::Payed:
        // not here
        break;
    }
}

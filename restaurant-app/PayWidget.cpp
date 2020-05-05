#include "PayWidget.hpp"
#include "OrderModel.hpp"

#include <QtCore/QString>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets\QMessageBox>

#include <QtCore/QAbstractTableModel>

#include "Model.hpp"

PayWidget::PayWidget(QWidget *parent) : QWidget(parent) {

    auto layout = new QVBoxLayout;

    auto selectFood = new QTableView;
    model_ = new OrderModel(selectFood);
    selectFood->setModel(model_);
    selectFood->setStyleSheet(" QTableViewEdit {border-style: outset}");

    payButton_ = new QPushButton(tr("Pay"));

    allPrice_ = new QLabel(tr("Price: "));

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
}

void PayWidget::payFailed() {
    payButton_->setEnabled(true);
}

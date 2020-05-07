#include "OrderDetailsWidget.hpp"

#include <QtWidgets/QDataWidgetMapper>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>

#include "OrdersModel.hpp"
#include "RequestedFoodModel.hpp"

OrderDetailsWidget::OrderDetailsWidget(QDataWidgetMapper *mapper, QWidget *parent) : QWidget(parent), mapper_(mapper) {
    orders_ = new RequestedFoodModel;

    auto layout = new QVBoxLayout;
    auto labelTableId = new QLabel;
    auto labelDate = new QLabel;
    auto labelStatus = new QLabel;

    auto selectedOrder = new QTableView;
    selectedOrder->setSelectionMode(QTableView::SelectionMode::NoSelection);
    selectedOrder->setModel(orders_);

    auto labelLayout = new QHBoxLayout;
    labelLayout->addWidget(labelTableId);
    labelLayout->addWidget(labelDate);
    labelLayout->addWidget(labelStatus, 0, Qt::AlignRight);

    inProgressButton_ = new QPushButton(tr("In Progress"));
    completedButton_ = new QPushButton(tr("Completed"));
    payedButton_ = new QPushButton(tr("Payed"));

    layout->addLayout(labelLayout);
    layout->addWidget(selectedOrder);
    layout->addWidget(inProgressButton_);
    layout->addWidget(completedButton_);
    layout->addWidget(payedButton_);

    setLayout(layout);

    mapper_->addMapping(labelTableId, 0, "text");
    mapper_->addMapping(labelDate, 1, "text");
    mapper_->addMapping(labelStatus, 2, "text");
    mapper_->addMapping(this, 3, "foodList");
    mapper_->revert();

    connect(inProgressButton_, &QPushButton::clicked, this, [this] { inProgress(); });
    connect(completedButton_, &QPushButton::clicked, this, [this] { completed(); });
    connect(payedButton_, &QPushButton::clicked, this, [this] { payed(); });
}

std::vector<FoodContains> OrderDetailsWidget::foodList() const {
    return orders_->foodList();
}

void OrderDetailsWidget::setFoodList(const std::vector<FoodContains> &value) {
    orders_->setFoodList(value);
}

void OrderDetailsWidget::orderChanged() {
    auto actualStatus = mapper_->model()->index(mapper_->currentIndex(), 2).data(Qt::UserRole).value<OrderStatus>();
    inProgressButton_->setEnabled(actualStatus == OrderStatus::Pending);
    completedButton_->setEnabled(actualStatus == OrderStatus::InProgress);
    payedButton_->setEnabled(actualStatus == OrderStatus::PayIntent);
}

#include "OrderDetailsWidget.hpp"

#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDataWidgetMapper>

#include "RequestedFoodModel.hpp"
#include "OrdersModel.hpp"

OrderDetailsWidget::OrderDetailsWidget(QDataWidgetMapper *mapper, QWidget *parent) : QWidget(parent) {
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

    auto inProgressButton = new QPushButton(tr("In Progress"));
    auto complatedButton = new QPushButton(tr("Complated"));
    auto payedButton = new QPushButton(tr("Payed"));

    layout->addLayout(labelLayout);
    layout->addWidget(selectedOrder);
    layout->addWidget(inProgressButton);
    layout->addWidget(complatedButton);
    layout->addWidget(payedButton);

    setLayout(layout);


    mapper->addMapping(labelTableId, 0, "text");
    mapper->addMapping(labelDate, 1, "text");
    mapper->addMapping(labelStatus, 2, "text");
    mapper->addMapping(this, 3, "foodList");
    mapper->revert();

    connect(mapper, &QDataWidgetMapper::currentIndexChanged, this,
            [this, mapper, inProgressButton, complatedButton, payedButton](int index) {
               auto actualStatus = mapper->model()->index(index, 2).data(Qt::UserRole).value<OrderStatus>();
                inProgressButton->setEnabled(actualStatus == OrderStatus::Pending);
                complatedButton->setEnabled(actualStatus == OrderStatus::InProgress);
                payedButton->setEnabled(actualStatus == OrderStatus::Complated);
            });

    connect(inProgressButton, &QPushButton::clicked, this, [this]{ 
        inProgress(); });
    connect(complatedButton, &QPushButton::clicked, this, [this] { complated(); });
    connect(payedButton, &QPushButton::clicked, this, [this] { payed(); });
}

std::vector<FoodContains> OrderDetailsWidget::foodList() const {
    return orders_->foodList();
}

void OrderDetailsWidget::setFoodList(const std::vector<FoodContains> &value) {
    orders_->setFoodList(value);
}

#include "OrderWidget.hpp"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>

#include "CartModel.hpp"
#include "MenuModel.hpp"

OrderWidget::OrderWidget(QWidget *parent) : QWidget(parent) {
    foodListModel_ = new MenuModel(this);
    cartModel_ = new CartModel(this);

    auto mainLayout = new QHBoxLayout;

    auto selectFood = new QTableView;
    selectFood->setSelectionBehavior(QTableView::SelectRows);
    selectFood->setSelectionMode(QTableView::MultiSelection);
    selectFood->setModel(foodListModel_);

    auto cartView = new QTableView;
    cartView->setModel(cartModel_);
    cartView->setSelectionBehavior(QTableView::SelectRows);
    cartView->setSelectionMode(QTableView::MultiSelection);

    auto cartButton = new QPushButton(tr("Add to cart"));
    auto deleteButton = new QPushButton(tr("Delete Food"));
    auto clearButton = new QPushButton(tr("Clear Selected Foods"));
    auto newOrderButton = new QPushButton(tr("Order"));

    mainLayout->addWidget(selectFood, 2);

    auto cartLayout = new QVBoxLayout;
    cartLayout->addWidget(cartView);
    cartLayout->addWidget(cartButton);
    cartLayout->addWidget(deleteButton);
    cartLayout->addWidget(clearButton);
    cartLayout->addWidget(newOrderButton);

    mainLayout->addLayout(cartLayout, 1);

    setLayout(mainLayout);
    connect(cartButton, &QPushButton::clicked, this, [this, selectFood] {
        auto selectionModel = selectFood->selectionModel();
        for (auto index : selectionModel->selectedRows()) {
            auto foodId = index.data(Qt::UserRole).value<std::uint32_t>();
            cartModel_->incrementAmount(foodId);
        }
    });
    // ToDo
    connect(deleteButton, &QPushButton::clicked, this, [this, cartView] {
        for (auto index : cartView->selectionModel()->selectedRows()) {
            cartModel_->removeRow(index.row());
        }
    });

    connect(clearButton, &QPushButton::clicked, this, [this] { cartModel_->clearSelectedFoods(); });

    connect(newOrderButton, &QPushButton::clicked, this, [this] {
        for (const FoodContains &item : cartModel_->selectedFoods()) {
            orderRequestFoods.push_back({item.FoodId, item.Amount});
        }
        orderRequested(orderRequestFoods);
    });
}

void OrderWidget::setFoodList(const std::vector<FoodContains> &foodList) {
    foodListModel_->setFoodList(foodList);
    cartModel_->setAvailableFoods(foodList);
}

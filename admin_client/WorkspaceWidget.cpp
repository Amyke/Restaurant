#include "WorkspaceWidget.hpp"

#include <QDataWidgetMapper>
#include <QtCore/QSortFilterProxyModel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTableView>

#include "AdministrationWidget.hpp"
#include "DiagrammWidget.hpp"
#include "Model.hpp"
#include "OrderDetailsWidget.hpp"
#include "OrdersModel.hpp"

WorkspaceWidget::WorkspaceWidget(Model &model, QWidget *parent) : QSplitter(parent), model_(model) {
    ordersModel_ = new OrdersModel(this);

    auto orderListView = new QTableView;
    orderListView->setModel(ordersModel_);
    orderListView->setColumnHidden(3, true);
    orderListView->setColumnHidden(4, true);

    auto mapper = new QDataWidgetMapper(this);
    mapper->setModel(ordersModel_);
    mapper->toFirst();

    auto administrationView = new AdministrationWidget;
    auto diagrammView = new DiagrammWidget;
    auto orderDetailsView = new OrderDetailsWidget(mapper);

    toolbox_ = new QStackedWidget;
    toolbox_->addWidget(orderDetailsView);
    toolbox_->addWidget(administrationView);
    toolbox_->addWidget(diagrammView);

    addWidget(orderListView);
    addWidget(toolbox_);

    connect(&model, &Model::loginSucceded, this, [this, &model] { model.complateListFoodRequest(); });

    connect(&model, &Model::newOrderArrived, this,
            [this, orderListView](const std::vector<Orders> &value) { ordersModel_->setArrivedOrders(value); });

    connect(orderListView->selectionModel(), &QItemSelectionModel::currentRowChanged, mapper,
            [mapper](const QModelIndex &current, const auto &) {
                // asd
                mapper->setCurrentIndex(current.row());
            });

    connect(orderDetailsView, &OrderDetailsWidget::inProgress, this, [this, orderListView, &model] {
        auto actualIndex = orderListView->selectionModel()->currentIndex().siblingAtColumn(4);
        auto orderId = actualIndex.data(Qt::UserRole).value<std::uint64_t>();
        auto status = OrderStatus::InProgress;
        model.orderStatusChangeRequest(orderId, status);
    });

    connect(orderDetailsView, &OrderDetailsWidget::complated, this, [this, orderListView, &model] {
        auto actualIndex = orderListView->selectionModel()->currentIndex().siblingAtColumn(4);
        auto orderId = actualIndex.data(Qt::UserRole).value<std::uint64_t>();
        auto status = OrderStatus::Complated;
        model.orderStatusChangeRequest(orderId, status);
    });
    connect(orderDetailsView, &OrderDetailsWidget::payed, this, [this, orderListView, &model] {
        auto actualIndex = orderListView->selectionModel()->currentIndex().siblingAtColumn(4);
        auto orderId = actualIndex.data(Qt::UserRole).value<std::uint64_t>();
        auto status = OrderStatus::Payed;
        model.orderStatusChangeRequest(orderId, status);
    });

    connect(&model, &Model::orderStatusChangeSucceded, this,
            [this](std::uint64_t orderId, OrderStatus status, std::uint64_t date) { ordersModel_->setStatus(orderId, status, date); });

    connect(&model, &Model::foodListRefreshed, this, [this, administrationView](const std::vector<Food> &foodList) {
        administrationView->setFoodList(foodList);
        toolbox_->setCurrentWidget(administrationView);
    });

    connect(administrationView, &AdministrationWidget::changedFood, this,
            [this, &model](const Delta &value) { model.foodChangeRequest(value); });

    connect(&model, &Model::foodChangeSucceded, this, [this, &model, administrationView] {
        QMessageBox::information(administrationView, tr("Food change success"),
                                 tr("the table is going to be refreshed"), QMessageBox::StandardButton::Ok);
        model.complateListFoodRequest();
    });

    connect(&model, &Model::foodChangeFailed, this, [this] {
        QMessageBox::critical(this, tr("Food change error"),
                              tr("The food change is failed, pls try again or ask help"));
    });

    connect(&model, &Model::orderStatusChangeFailed, this, [this] {
        QMessageBox::critical(this, tr("Order Status error"),
                              tr("The order status change is failed, pls try again or ask help"));
    });
}

void WorkspaceWidget::requestAdministration() {
    toolbox_->setCurrentIndex(1);
}

void WorkspaceWidget::requestDiagramm() {
    toolbox_->setCurrentIndex(2);
}

void WorkspaceWidget::requestOrderDetails() {
    toolbox_->setCurrentIndex(0);
}

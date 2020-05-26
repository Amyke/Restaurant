#include "WorkspaceWidget.hpp"

#include <QDataWidgetMapper>
#include <QtCore/QDateTime>
#include <QtCore/QSortFilterProxyModel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTableView>

#include "AdministrationWidget.hpp"
#include "DiagrammWidget.hpp"
#include "Model.hpp"
#include "OrderDetailsWidget.hpp"
#include "OrdersModel.hpp"

struct FilterOutPayedStatusProxyModel : QSortFilterProxyModel {
    using QSortFilterProxyModel::QSortFilterProxyModel;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const final {
        auto status = sourceModel()->index(source_row, 2).data(Qt::UserRole).value<OrderStatus>();
        return status != OrderStatus::Payed;
    }
};

WorkspaceWidget::WorkspaceWidget(Model &model, QWidget *parent) : QSplitter(parent), model_(model) {
    ordersModel_ = new OrdersModel(this);

    auto orderListView = new QTableView;
    auto proxyModel = new FilterOutPayedStatusProxyModel(orderListView);
    proxyModel->setSourceModel(ordersModel_);
    orderListView->setModel(proxyModel);
    orderListView->setSortingEnabled(true);
    orderListView->setColumnHidden(3, true);
    orderListView->setColumnHidden(4, true);
    orderListView->sortByColumn(1, Qt::DescendingOrder);

    auto mapper = new QDataWidgetMapper(this);
    mapper->setModel(proxyModel);
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

    connect(&model, &Model::loginSucceded, this, [&model] { model.completeListRequest(); });

    connect(&model, &Model::loginSucceded, this, [&model] {
        auto now = QDateTime::currentDateTime();
        auto oneWeekAgo = now.addDays(-7);
        model.ordersListRequest(oneWeekAgo.toTime_t(), now.toTime_t());
    });

    connect(&model, &Model::orderListArrived, this, [this, orderDetailsView, diagrammView](const std::vector<Orders> &value) {
        ordersModel_->addOrders(value);

        diagrammView->refreshData(ordersModel_->orders());
    });

    connect(&model, &Model::newOrderArrived, this, [this, orderDetailsView](const Orders &value) {
        ordersModel_->addOrders({value});
        orderDetailsView->orderChanged();
    });

    connect(orderListView->selectionModel(), &QItemSelectionModel::currentRowChanged, mapper,
            [mapper, orderDetailsView](const QModelIndex &current, const auto &) {
                mapper->setCurrentIndex(current.row());
                orderDetailsView->orderChanged();
            });

    connect(orderDetailsView, &OrderDetailsWidget::inProgress, this, [mapper, proxyModel, &model] {
        auto actualIndex = proxyModel->index(mapper->currentIndex(), 4);
        auto orderId = actualIndex.data(Qt::UserRole).value<std::uint64_t>();
        auto status = OrderStatus::InProgress;
        model.orderStatusChangeRequest(orderId, status);
    });

    connect(orderDetailsView, &OrderDetailsWidget::completed, this, [orderListView, &model] {
        auto actualIndex = orderListView->selectionModel()->currentIndex().siblingAtColumn(4);
        auto orderId = actualIndex.data(Qt::UserRole).value<std::uint64_t>();
        auto status = OrderStatus::Completed;
        model.orderStatusChangeRequest(orderId, status);
    });
    connect(orderDetailsView, &OrderDetailsWidget::payed, this, [orderListView, &model] {
        auto actualIndex = orderListView->selectionModel()->currentIndex().siblingAtColumn(4);
        auto orderId = actualIndex.data(Qt::UserRole).value<std::uint64_t>();
        auto status = OrderStatus::Payed;
        model.orderStatusChangeRequest(orderId, status);
    });

    connect(diagrammView, &DiagrammWidget::dataRequest, this, [&model](const QDate &begin, const QDate &end) {
        model.ordersListRequest(((QDateTime)begin).toTime_t(), ((QDateTime)end).toTime_t());
    });

    connect(&model, &Model::orderStatusChangeSucceded, this,
            [this, orderDetailsView](std::uint64_t orderId, OrderStatus status, std::uint64_t date) {
                ordersModel_->setStatus(orderId, status, date);
                orderDetailsView->orderChanged();
            });

    connect(&model, &Model::foodListRefreshed, this, [this, administrationView](const std::vector<Food> &foodList) {
        administrationView->setFoodList(foodList);
        toolbox_->setCurrentWidget(administrationView);
    });

    connect(administrationView, &AdministrationWidget::changedFood, this,
            [&model](const Delta &value) { model.foodChangeRequest(value); });

    connect(&model, &Model::foodChangeSucceded, this, [&model, administrationView] {
        QMessageBox::information(administrationView, tr("Food change success"),
                                 tr("the table is going to be refreshed"), QMessageBox::StandardButton::Ok);
        model.completeListRequest();
    });

    connect(&model, &Model::foodChangeFailed, this, [this] {
        QMessageBox::critical(this, tr("Food change error"),
                              tr("The food change is failed, please try again or ask help"));
    });

    connect(&model, &Model::orderStatusChangeFailed, this, [this] {
        QMessageBox::critical(this, tr("Order Status error"),
                              tr("The order status change is failed, please try again or ask help"));
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

#include "OrdersModel.hpp"

#include <QDateTime>

OrdersModel::OrdersModel(QObject *parent) : QAbstractTableModel(parent) {
    std::vector<FoodContains> foods{FoodContains{0, "valami", 2, 5}};
    orders_ = {Orders{1, "TableId", foods, QDateTime::currentDateTime().toTime_t(), OrderStatus::Payed},
               Orders{2, "TableId2", {}, QDateTime::currentDateTime().toTime_t(), OrderStatus::Complated}};

    qRegisterMetaType<std::vector<FoodContains>>();
}

int OrdersModel::rowCount(const QModelIndex &parent) const {
    return orders_.size();
}

QVariant OrdersModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Table Id");
        case 1:
            return tr("Date");
        case 2:
            return tr("Status");
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

int OrdersModel::columnCount(const QModelIndex &parent) const {
    return 4;
}

QVariant OrdersModel::data(const QModelIndex &index, int role) const {
    if (role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::UserRole) {
        return {};
    }

    const auto &order = orders_[index.row()];
    switch (index.column()) {
    case 0:
        return QString::fromStdString(order.TableId);
    case 1:
        return QDateTime::fromTime_t(order.OrderedDate);
    case 2:
        if (role == Qt::UserRole) {
            return QVariant::fromValue(order.Status);
        } else {
            return orderStatusToString(order.Status);
        }
    case 3:
        if (role == Qt::EditRole) {
            return QVariant::fromValue(order.OrderedFoods);
        } else {
            return {};
        }
    case 4:
        if (role == Qt::EditRole || role == Qt::UserRole) {
            return QVariant::fromValue(order.OrderId);
        } else {
            return {};
        }
    }
    return {};
}

void OrdersModel::addOrders(const std::vector<Orders> &order) {
    beginResetModel();

    std::vector<Orders> toBeInserted;
    auto newIt = order.begin();
    for (auto originalIt = orders_.begin(); originalIt != orders_.end() && newIt != order.end();) {

        if (originalIt->OrderId == newIt->OrderId) {
            *originalIt = *newIt;
            ++originalIt, ++newIt;
        } else if (originalIt->OrderId < newIt->OrderId) {
            ++originalIt;
        } else if (originalIt->OrderId > newIt->OrderId) {
            toBeInserted.push_back(*newIt);
            ++newIt;
        }
    }

    orders_.insert(orders_.end(), newIt, order.end());
    orders_.insert(orders_.end(), toBeInserted.begin(), toBeInserted.end());

    std::sort(orders_.begin(), orders_.end(),
              [](const Orders &left, const Orders &right) { return left.OrderId < right.OrderId; });

    endResetModel();
}

void OrdersModel::setStatus(std::uint64_t orderId, OrderStatus status, std::uint64_t date) {
    // invariant: orders_ is sorted by id

    auto it = std::find_if(orders_.begin(), orders_.end(),
                           [orderId](const Orders &order) { return order.OrderId == orderId; });
    if (it == orders_.end()) {
        return;
    }

    it->Status = status;
    it->OrderedDate = date;
    auto row = std::distance(orders_.begin(), it);
    dataChanged(index(row, 2), index(row, 2));
}

QString OrdersModel::orderStatusToString(OrderStatus status) const {
    switch (status) {
    case OrderStatus::Pending:
        return tr("Pending");
    case OrderStatus::InProgress:
        return tr("In Progress");
    case OrderStatus::Complated:
        return tr("Complated");
    case OrderStatus::Payed:
        return tr("Payed");
    }
    return {};
}

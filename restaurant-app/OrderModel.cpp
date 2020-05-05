#include <QtCore/QAbstractTableModel>

#include "OrderModel.hpp"

OrderModel::OrderModel(QObject *parent) : QAbstractTableModel(parent) {
}

int OrderModel::rowCount(const QModelIndex &parent) const {
    return order.size();
}

int OrderModel::columnCount(const QModelIndex &parent) const {
    return 3;
}

QVariant OrderModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Name");
        case 1:
            return tr("Amount");
        case 2:
            return tr("Price");
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant OrderModel::data(const QModelIndex &index, int role) const {
    if (role != Qt::DisplayRole) {
        return {};
    }

    const auto &foods = order[index.row()];
    switch (index.column()) {
    case 0:
        return QString::fromStdString(foods.FoodName);
    case 1:
        return foods.Amount;
    case 2:
        return foods.FoodPrice * foods.Amount;
    }

    return {};
}

void OrderModel::setFoodList(const std::vector<FoodContains> &foodList) {
    beginResetModel();
    order = foodList;
    endResetModel();
}

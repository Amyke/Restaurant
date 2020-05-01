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

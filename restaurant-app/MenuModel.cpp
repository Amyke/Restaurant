#include "MenuModel.hpp"

#include <QtCore/QAbstractTableModel>

MenuModel::MenuModel(QObject *parent) : QAbstractTableModel(parent) {
}

int MenuModel::rowCount(const QModelIndex &parent) const {
    return order.size();
}

int MenuModel::columnCount(const QModelIndex &parent) const {
    return 3;
}

QVariant MenuModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Name");
        case 1:
            return tr("Price");
        case 2:
            return tr("Available");
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant MenuModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::UserRole) {
        return order[index.row()].FoodId;
    }

    if (role != Qt::DisplayRole) {
        return {};
    }

    const auto &food = order[index.row()];

    switch (index.column()) {
    case 0:
        return QString::fromStdString(food.FoodName);
    case 1:
        return food.FoodPrice;
    case 2:
        return food.Amount;
    }

    return {};
}

void MenuModel::setFoodList(const std::vector<FoodContains> &foodList) {
    beginResetModel();
    order = foodList;
    endResetModel();
}

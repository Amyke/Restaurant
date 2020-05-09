#include "CartModel.hpp"

#include <QtGui/QColor>

CartModel::CartModel(QObject *parent) : QAbstractTableModel(parent) {
}

int CartModel::rowCount(const QModelIndex &parent) const {
    return selectedFoods_.size();
}

int CartModel::columnCount(const QModelIndex &parent) const {
    return 3;
}

QVariant CartModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Name");
        case 1:
            return tr("Price");
        case 2:
            return tr("Amount");
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant CartModel::data(const QModelIndex &index, int role) const {
    if (role != Qt::DisplayRole && role != Qt::BackgroundRole) {
        return {};
    }

    const auto &food = selectedFoods_[index.row()];
    if (role == Qt::BackgroundRole) {
        auto search = std::find_if(foods_.begin(), foods_.end(),
                                   [id = food.FoodId](const FoodContains &food) { return food.FoodId == id; });
        if (search == foods_.end() || search->Amount < food.Amount) {
            return QColor(Qt::red);
        } else {
            return {};
        }
    }

    switch (index.column()) {
    case 0:
        return QString::fromStdString(food.FoodName);
    case 1:
        return tr("%1 (%2 / pc)").arg(food.Amount * food.FoodPrice).arg(food.FoodPrice);
    case 2:
        return food.Amount;
    }

    return {};
}

bool CartModel::removeRows(int row, int count, const QModelIndex &parent) {
    beginRemoveRows(QModelIndex(), row, row + count - 1);

    auto it = selectedFoods_.begin() + row;
    selectedFoods_.erase(it, it + count);

    endRemoveRows();
    return true;
}

void CartModel::clearSelectedFoods() {
    beginResetModel();

    selectedFoods_.clear();

    endResetModel();
}

void CartModel::setAvailableFoods(const std::vector<FoodContains> &value) {
    beginResetModel();
    foods_ = value;
    endResetModel();
}

void CartModel::incrementAmount(std::uint32_t foodId) {
    auto selectedSearch = std::find_if(selectedFoods_.begin(), selectedFoods_.end(),
                                       [foodId](const FoodContains &f) { return f.FoodId == foodId; });
    if (selectedSearch != selectedFoods_.end()) {
        selectedSearch->Amount += 1;
        auto row = std::distance(selectedFoods_.begin(), selectedSearch);
        dataChanged(index(row, 0), index(row, 2));
        return;
    }

    auto search =
        std::find_if(foods_.begin(), foods_.end(), [foodId](const FoodContains &f) { return f.FoodId == foodId; });
    if (search == foods_.end()) {
        return;
    }

    beginInsertRows({}, selectedFoods_.size(), selectedFoods_.size());
    FoodContains food = *search;
    food.Amount = 1;
    selectedFoods_.push_back(food);
    endInsertRows();
}

#include "FoodModel.hpp"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>

#include <QtCore/QAbstractTableModel>

#include <algorithm>

FoodModel::FoodModel(QObject *parent) : QAbstractTableModel(parent) {
    listFoods = {{1, "Kaja", 1, 2, true}};
    foodChanges = listFoods;
}

int FoodModel::rowCount(const QModelIndex &parent) const {
    return foodChanges.size();
}

int FoodModel::columnCount(const QModelIndex &parent) const {
    return 4;
}

QVariant FoodModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Name");
        case 1:
            return tr("Price");
        case 2:
            return tr("Amount");
        case 3:
            return tr("Visible");
        }
    }
    if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        const auto &food = foodChanges[section];
        if (food.FoodData.FoodId == 0) {
            return "*";
        }
        return {};
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant FoodModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::UserRole) {
        return foodChanges[index.row()].FoodData.FoodId;
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole) {
        return {};
    }

    const auto &food = foodChanges[index.row()];

    switch (index.column()) {
    case 0:
        return QString::fromStdString(food.FoodData.FoodName);
    case 1:
        return food.FoodData.FoodPrice;
    case 2:
        return food.FoodData.Amount;
    case 3:
        return food.Visible;
    }

    return {};
}

bool FoodModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    switch (index.column()) {
    case 0:
        if (!value.canConvert<QString>()) {
            return false;
        }
        foodChanges[index.row()].FoodData.FoodName = value.toString().toStdString();
        return true;
    case 1:
        if (!value.canConvert<int>()) {
            return false;
        }
        foodChanges[index.row()].FoodData.FoodPrice = value.toInt();
        return true;
    case 2:
        if (!value.canConvert<int>()) {
            return false;
        }
        foodChanges[index.row()].FoodData.Amount = value.toInt();
        return true;
    case 3:
        if (!value.canConvert<bool>()) {
            return false;
        }
        foodChanges[index.row()].Visible = value.toBool();
        return true;
    }
    return false;
}

bool FoodModel::insertRows(int row, int count, const QModelIndex &parent) {
    if (row != rowCount() || count != 1) {
        return false;
    }
      
    beginInsertRows(QModelIndex(), row, row);

    foodChanges.push_back({0, "EditMe", 0, 0, false});

    endInsertRows();
    return true;
}

void FoodModel::setFoodList(const std::vector<Food> &foodList) {
    beginResetModel();
    listFoods = foodList;
    foodChanges = foodList;
    endResetModel();
}

template <typename Iterator, typename Predicate>
Iterator find_if(Iterator begin, Iterator end, Predicate pred) {
    for (; begin != end; ++begin) {
        if (predicate(*begin)) {
            return begin;
        }
    }
    return end;
}

Delta FoodModel::computeDelta() {
    // listFoods => original
    //  => ordered by id
    // foodChanges => copy of listFoods + additional items
    //  => first partition: ordered by id
    //  => second partition: I don't care about the id
    // listFoods.size == first partition size

    Delta delta;

    for (auto originalIt = listFoods.begin(), newIt = foodChanges.begin(); originalIt != listFoods.end();
         ++originalIt, ++newIt) {
        const Food &old = *originalIt;
        const Food &newer = *newIt;
        if (newer.FoodData.FoodName != old.FoodData.FoodName || newer.FoodData.FoodPrice != old.FoodData.FoodPrice ||
            newer.FoodData.Amount != old.FoodData.Amount || newer.Visible != old.Visible) {
            delta.ModifiedFoods.push_back(newer);
        }
    }

    for (auto it = foodChanges.begin() + listFoods.size(); it != foodChanges.end(); ++it) {
        delta.CreatedFoods.push_back(*it);
    }

    return delta;
}

void FoodModel::resetData() {
    beginResetModel();
    foodChanges = listFoods;
    endResetModel();
}

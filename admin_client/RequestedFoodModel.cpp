#include "RequestedFoodModel.hpp"

#include <QDateTime>
#include <QVariant>
#include <QtGlobal>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

#include "Message.hpp"

RequestedFoodModel::RequestedFoodModel(QObject *parent) : QAbstractTableModel(parent) {
}

int RequestedFoodModel::rowCount(const QModelIndex &parent) const {
    return foods_.size();
}

QVariant RequestedFoodModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Ordered foods");
        case 1:
            return tr("Amount");
        case 2:
            return tr("Sum Price/food");
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

int RequestedFoodModel::columnCount(const QModelIndex &parent) const {
    return 3;
}

QVariant RequestedFoodModel::data(const QModelIndex &index, int role) const {
    if (role != Qt::DisplayRole) {
        return {};
    }

    const auto &neworder = foods_[index.row()];
    switch (index.column()) {
    case 0:
        return QString::fromStdString(neworder.FoodName);
    case 1:
        return neworder.Amount;
    case 2:
        return neworder.FoodPrice * neworder.Amount;
    }
    return {};
}

std::vector<FoodContains> RequestedFoodModel::foodList() const {
    return foods_;
}

void RequestedFoodModel::setFoodList(const std::vector<FoodContains> & value) {
    beginResetModel();
    foods_ = value;
    endResetModel();
}

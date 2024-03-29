#ifndef ORDERMODEL_HPP
#define ORDERMODEL_HPP

#include <QAbstractTableModel>
#include <QString>

#include "Message.hpp"

class OrderModel : public QAbstractTableModel {

public:
    OrderModel(QObject *parent = nullptr);
    //: QAbstractTableModel(parent), order(ord) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setFoodList(const std::vector<FoodContains> &foodList);

private:
    std::vector<FoodContains> order;
};

#endif //! ORDERMODEL_HPP
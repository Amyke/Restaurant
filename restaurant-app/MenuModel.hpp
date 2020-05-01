#ifndef MENUMODEL_HPP
#define MENUMODEL_HPP

#include "Model.hpp"
#include <QAbstractTableModel>
#include <QString>

class MenuModel : public QAbstractTableModel {
    Q_OBJECT;

public:
    MenuModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setFoodList(const std::vector<FoodContains> &foodList);

public:
    std::vector<FoodContains> order;
};

#endif //! #ifndef MENUMODEL_HPP

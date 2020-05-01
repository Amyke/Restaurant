#ifndef REQUESTFOODMODEL_HPP
#define REQUESTFOODMODEL_HPP

#include "Message.hpp"
#include <QAbstractTableModel>
#include <QString>

class RequestedFoodModel : public QAbstractTableModel {
    Q_OBJECT
public:
    RequestedFoodModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    std::vector<FoodContains> foodList() const;
    void setFoodList(const std::vector<FoodContains> &);

private:
    std::vector<FoodContains> foods_;
};

#endif // !REQUESTFOODMODEL_HPP
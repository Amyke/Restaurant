#ifndef CARTMODEL_HPP
#define CARTMODEL_HPP

#include <vector>

#include <QtCore/QAbstractTableModel>

#include "Message.hpp"

class CartModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit CartModel(QObject *parent);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void clearSelectedFoods();

    void setAvailableFoods(const std::vector<FoodContains> &);
    void incrementAmount(std::uint32_t foodId);

    const std::vector<FoodContains> &selectedFoods() const {
        return selectedFoods_;
    }

private:
    std::vector<FoodContains> foods_;
    std::vector<FoodContains> selectedFoods_;
};

#endif // !CARTMODEL_HPP

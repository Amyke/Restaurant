#include <QAbstractTableModel>
#include <QString>

#include "Message.hpp"

class FoodModel : public QAbstractTableModel {
public:
    FoodModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override {
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void setFoodList(const std::vector<Food> &foodList);
    Delta computeDelta();

    void resetData();

private:
    std::vector<Food> listFoods;
    std::vector<Food> foodChanges;
};
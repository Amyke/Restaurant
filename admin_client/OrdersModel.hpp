#ifndef ORDERSMODEL_HPP
#define ORDERSMODEL_HPP

#include <QtCore/QAbstractTableModel>

#include "Model.hpp"

Q_DECLARE_METATYPE(std::vector<FoodContains>);
Q_DECLARE_METATYPE(OrderStatus);

class OrdersModel : public QAbstractTableModel {
    Q_OBJECT
public:
    OrdersModel(QObject *parent = nullptr);
 
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    const std::vector<Orders> &orders() const {
        return orders_;
    }
    
    static void mergeOrders(std::vector<Orders> &present, const std::vector<Orders> &newOrders);
    void addOrders(const std::vector<Orders> &order);

    void setStatus(std::uint64_t orderId, OrderStatus status, std::uint64_t date);

private:
    QString orderStatusToString(OrderStatus status) const;
    
    std::vector<Orders> orders_;
};

#endif //! ORDERSMODEL_HPP
#include "OrdersModel.hpp"

#include <QtTest/QtTest>

Q_DECLARE_METATYPE(std::vector<Orders>);

char *toString(OrderStatus status) {
    using QTest::toString;
    switch (status) {
    case OrderStatus::Pending:
        return toString("Pending");
    case OrderStatus::InProgress:
        return toString("InProgress");
    case OrderStatus::Completed:
        return toString("Completed");
    case OrderStatus::Payed:
        return toString("Payed");
    case OrderStatus::PayIntent:
        return toString("PayIntent");
    }
    return toString(QString("<INVALID>(%1)").arg(static_cast<std::underlying_type_t<OrderStatus>>(status)));
}

bool operator==(const FoodContains &lhs, const FoodContains &rhs) {
    return lhs.FoodId == rhs.FoodId && lhs.FoodName == rhs.FoodName && lhs.Amount == rhs.Amount &&
           lhs.FoodPrice == rhs.FoodPrice;
}

bool operator==(const Orders &lhs, const Orders &rhs) {
    return lhs.OrderId == rhs.OrderId && lhs.TableId == rhs.TableId &&
           std::equal(lhs.OrderedFoods.begin(), lhs.OrderedFoods.end(), rhs.OrderedFoods.begin(),
                      rhs.OrderedFoods.end()) &&
           lhs.OrderedDate == rhs.OrderedDate && lhs.Status == rhs.Status;
}

struct OrderIdLess {
    bool operator()(const Orders &lhs, const Orders &rhs) const {
        return lhs.OrderId < rhs.OrderId;
    }
};

class OrdesModelTest : public QObject {
    Q_OBJECT
private slots:
    void initTestCase() {
        qRegisterMetaType<std::vector<Orders>>();
    }

    void init() {
        orders_ = new OrdersModel;
    }
    void cleanup() {
        delete orders_;
    }

    void AddOrdersSimpleSimulate() {
        std::vector<Orders> order;
        std::vector<FoodContains> foods;
        foods.push_back({1, "Palacsinta", 2, 500});
        order.push_back({0, "Table1", foods, QDateTime::currentDateTime().toTime_t(), OrderStatus::Pending});
        order.push_back({1, "Table2", foods, QDateTime::currentDateTime().toTime_t(), OrderStatus::Pending});
        order.push_back({2, "Table3", foods, QDateTime::currentDateTime().toTime_t(), OrderStatus::Pending});
        order.push_back({3, "Table3", foods, QDateTime::currentDateTime().toTime_t(), OrderStatus::Pending});

        std::vector<Orders> old;
        OrdersModel::mergeOrders(old, order);

        QCOMPARE(old, order);
    }

    void AddOrdersSortSimulate() {
        std::vector<Orders> order;
        std::vector<FoodContains> foods;
        foods.push_back({1, "Palacsinta", 2, 500});
        order.push_back({1, "Table2", foods, QDateTime::currentDateTime().toTime_t(), OrderStatus::Pending});
        order.push_back({3, "Table4", foods, QDateTime::currentDateTime().toTime_t(), OrderStatus::Pending});
        order.push_back({0, "Table1", foods, QDateTime::currentDateTime().toTime_t(), OrderStatus::Pending});
        order.push_back({2, "Table3", foods, QDateTime::currentDateTime().toTime_t(), OrderStatus::Pending});

        std::vector<Orders> old;
        OrdersModel::mergeOrders(old, order);

        QVERIFY(std::is_sorted(old.begin(), old.end(), OrderIdLess{}));
    }

    void mergeOrders_overwritesOldValue() {
        std::vector<Orders> old = {Orders{0, "Table1", {}, {}, OrderStatus::InProgress}};
        std::vector<Orders> new_ = {Orders{0, "Table2", {}, {}, OrderStatus::Payed}};

        OrdersModel::mergeOrders(old, new_);

        QCOMPARE(old, new_);
    }

    void mergeOrders_keepsUnmodifiedValues_andAppendsAtTheEnd() {
        Orders order1{0, "Table1", {}, {}, OrderStatus::Payed};
        Orders order2{1, "Table2", {}, {}, OrderStatus::Completed};
        std::vector<Orders> old = {order1};
        std::vector<Orders> new_ = {order2};

        OrdersModel::mergeOrders(old, new_);

        std::vector<Orders> expected = {order1, order2};
        QCOMPARE(old, expected);
    }

    void mergeOrders_keepsUnmodifiedValues_andInsertsInTheMiddle() {
        Orders order1{0, "Table1", {}, {}, OrderStatus::Payed};
        Orders order2{1, "Table2", {}, {}, OrderStatus::Completed};
        Orders order3{3, "Table3", {}, {}, OrderStatus::InProgress};
        std::vector<Orders> old = {order1, order3};
        std::vector<Orders> new_ = {order2};

        OrdersModel::mergeOrders(old, new_);

        std::vector<Orders> expected = {order1, order2, order3};
        QCOMPARE(old, expected);
    }

    void mergeOrders_keepsUnmodifiedValues_andPrependsAtTheBeginning() {
        Orders order1{0, "Table1", {}, {}, OrderStatus::Payed};
        Orders order2{1, "Table2", {}, {}, OrderStatus::Completed};
        std::vector<Orders> old = {order2};
        std::vector<Orders> new_ = {order1};

        OrdersModel::mergeOrders(old, new_);

        std::vector<Orders> expected = {order1, order2};
        QCOMPARE(old, expected);
    }

    void mergeOrders_keepsUnmodifiedValues() {
        Orders order1{0, "Table1", {}, {}, OrderStatus::Payed};
        Orders order2{1, "Table2", {}, {}, OrderStatus::Completed};
        std::vector<Orders> old = {order1, order2};
        std::vector<Orders> new_ = {};

        OrdersModel::mergeOrders(old, new_);

        std::vector<Orders> expected = old;
        QCOMPARE(old, expected);
    }

    void SetStatusSimulate() {
        // std::uint64_t orderId, OrderStatus status, std::uint64_t date
        std::vector<Orders> order;
        std::vector<FoodContains> foods;
        foods.push_back({1, "Palacsinta", 2, 500});
        order.push_back({1, "Table2", foods, QDateTime::currentDateTime().toTime_t(), OrderStatus::Pending});
        order.push_back({3, "Table4", foods, QDateTime::currentDateTime().toTime_t(), OrderStatus::Pending});

        orders_->addOrders(order);

        orders_->setStatus(1, OrderStatus::InProgress, QDateTime::currentDateTime().toTime_t());
        orders_->setStatus(3, OrderStatus::Completed, QDateTime::currentDateTime().toTime_t());

        QCOMPARE(orders_->index(0, 2).data(Qt::UserRole).value<OrderStatus>(), OrderStatus::InProgress);
        QCOMPARE(orders_->index(1, 2).data(Qt::UserRole).value<OrderStatus>(), OrderStatus::Completed);
    }

private:
    OrdersModel *orders_;
};
QTEST_MAIN(OrdesModelTest);
#include "ordermodel_test.moc"
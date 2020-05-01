#ifndef MODEL_HPP
#define MODEL_HPP

#include <memory>
#include <vector>

#include <QString>
#include <QtCore/QObject>

#include "Client.hpp"
#include "Message.hpp"

class QTcpSocket;

Q_DECLARE_METATYPE(Orders);

class Model : public QObject {
    Q_OBJECT

    enum class State { WaitingForLogin, Working };
    friend QDebug &operator<<(QDebug &, State);

public:
    explicit Model(IClient *client, QObject *parent = nullptr);

    ~Model();

    bool isConnected() const {
        return connected_;
    }

    void connectToServer(const QString &host, quint32 port);
    void login(const QString &username, const QString &password);

    void ordersListRequest(std::uint64_t fromDate, std::uint64_t toDate);

    void complateListRequest();
    void foodChangeRequest(const Delta &delta);

    void orderStatusChangeRequest(std::uint64_t orderId, OrderStatus status);

signals:
    void connectionStateChanged(bool isOnline);
    void loginSucceded();
    void loginFailed();

    void orderListArrived(const std::vector<Orders> &value);

    void newOrderArrived(const Orders &value);

    void orderStatusChangeSucceded(std::uint64_t orderId, OrderStatus status, std::uint64_t date);
    void orderStatusChangeFailed(std::uint64_t orderId, OrderStatus status, std::uint64_t date);

    void foodListRefreshed(const std::vector<Food> &value);
    void foodChangeSucceded();
    void foodChangeFailed();

    void diagrammDataArrived();

    void exited();

private:
    void on_connected();

    void handleMessageArrived(QSharedPointer<Message> msg);
    void handleLoginReply(const LoginReplyMessage &msg);
    
    void handleOrderListReply(const OrderArrivedReplyMessage &msg);

    void handleListFoodReply(const ComplateFoodReplyMesage &msg);
    void handleFoodChangeReply(const FoodChangeReplyMessage &msg);

    void handleNotificationOrders(const NotificationOrdersMessage &msg);
    void handleOrderStatusChangeReply(const OrderStatusChangeReplyMessage &status);

    void ActualStateChange(const State &state);

private:
    IClient *client;
    bool connected_ = false;
    State actualState = State::WaitingForLogin;
    std::vector<Food> availableFoods_;
};

#endif //! MODEL_HPP
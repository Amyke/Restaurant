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

    void complateListFoodRequest();
    void foodChangeRequest(const Delta &delta);

    void orderStatusChangeRequest(std::uint64_t orderId, OrderStatus status);

signals:
    void connectionStateChanged(bool isOnline);
    void loginSucceded();
    void loginFailed();

    void newOrderArrived(const std::vector<Orders> &value);

    void orderStatusChangeSucceded(std::uint64_t orderId, OrderStatus status);
    void orderStatusChangeFailed(std::uint64_t orderId, OrderStatus status);

    void foodListRefreshed(const std::vector<Food> &value);
    void foodChangeSucceded();
    void foodChangeFailed();

    void diagrammDataArrived();

    void exited();

private:
    void on_connected();

    void handleMessageArrived(QSharedPointer<Message> msg);
    void handleLoginReply(const LoginReplyMessage &msg);

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
    std::vector<Orders> requestedOrders_;
};

#endif //! MODEL_HPP
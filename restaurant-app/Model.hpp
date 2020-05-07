#ifndef MODEL_HPP
#define MODEL_HPP

#include <memory>

#include <QString>
#include <QtCore/QObject>

#include "Client.hpp"
#include "Message.hpp"

class QTcpSocket;
class QTimer;

class Model : public QObject {
    Q_OBJECT

    enum class State { WaitingForLogin, TitleScreen, WaitingForOrder, WaitingForPayIntent };
    friend QDebug &operator<<(QDebug &, State);

public:
    explicit Model(IClient *client, QObject *parent = nullptr);

    ~Model();

    bool isConnected() const {
        return connected_;
    }

    void connectToServer(const QString &host, quint32 port);
    void login(const QString &username, const QString &password);
    void listFoodRequest();
    void orderSend(const std::vector<FoodAmount> &foods);
    void paySend();
    void exit();

signals:
    void connectionStateChanged(bool isOnline);
    void loginSucceded();
    void loginFailed();

    void foodListRefreshed(const std::vector<FoodContains> &);

    void orderSucceded(const std::vector<FoodContains> &);
    void orderFailed();

    void paySucceded();
    void payFailed();

    void statusChanged(OrderStatus status);

    void exited();

private:
    void on_connected();
    void handleMessageArrived(QSharedPointer<Message> msg);

    void handleLoginReply(const LoginReplyMessage &msg);

    void handleListFoodReply(const FoodListReplyMessage &msg);
    void handleOrderReply(const OrderReplyMessage &msg);
    void handlePayReply(const PayReplyMessage &msg);

    void handleNotificationOrders(const NotificationOrdersMessage &msg);

    void actualStateChange(std::optional<std::uint64_t> activeOrderId = std::nullopt);

private:
    IClient *client;
    QTimer *reconnectTimer_;
    QMetaObject::Connection timeoutConnection_;
    bool connected_ = false;
    State actualState = State::WaitingForLogin;

    std::vector<FoodContains> availableFoods;

    std::optional<std::uint64_t> activeOrderId_;
    std::optional<OrderStatus> activeOrderStatus_;
};

#endif //! MODEL_HPP
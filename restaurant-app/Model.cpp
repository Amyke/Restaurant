#include "Model.hpp"

#include <QtCore/QDebug>
#include <QtCore/QTimer>

QDebug &operator<<(QDebug &os, Model::State state) {
    switch (state) {
    case Model::State::WaitingForLogin:
        return os << "WaitingForLogin";
    case Model::State::TitleScreen:
        return os << "TitleScreen";
    case Model::State::WaitingForFoodList:
        return os << "WaitingForFoodList";
    case Model::State::WaitingForOrder:
        return os << "WaitingForOrder";
    case Model::State::WaitingForPayIntent:
        return os << "WaitingForPayIntent";
    }
    return os << "<INVALID>";
}

QDebug &operator<<(QDebug &os, MessageId id) {
    auto sv = to_string(id);
    return os << QString::fromLatin1(sv.data(), sv.size());
}

Model::Model(IClient *client, QObject *parent) : QObject(parent), client(client), reconnectTimer_(new QTimer(this)) {
    reconnectTimer_->setInterval(500);

    connect(client, &IClient::messageArrived, this, &Model::handleMessageArrived);
    connect(client, &IClient::connectionEstablished, this, [this] {
        if (timeoutConnection_) {
            reconnectTimer_->stop();
        }
        connected_ = true;
        actualState = State::WaitingForLogin;
        connectionStateChanged(connected_);
    });
    connect(client, &IClient::connectionLost, this, [this] {
        connected_ = false;
        connectionStateChanged(connected_);
        if (timeoutConnection_) {
            reconnectTimer_->start();
        }
    });
}

Model::~Model() = default;

void Model::connectToServer(const QString &host, quint32 port) {
    client->connectToHost(host, port);
    if (timeoutConnection_) {
        disconnect(timeoutConnection_);
    }
    timeoutConnection_ =
        connect(reconnectTimer_, &QTimer::timeout, this, [this, host, port] { connectToServer(host, port); });
}

void Model::login(const QString &username, const QString &password) {
    if (!connected_) {
        qWarning() << __FUNCTION__ << "called while the connection is not alive!";
        return;
    }

    if (actualState != State::WaitingForLogin) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    auto msg = QSharedPointer<LoginRequestMessage>::create(username.toStdString(), password.toStdString(), false);

    client->send(msg);
}

void Model::listFoodRequest() {
    if (!connected_) {
        qWarning() << __FUNCTION__ << "called while the connection is not alive!";
        return;
    }

    if (actualState != State::TitleScreen) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    auto msg = QSharedPointer<FoodListRequestMessage>::create();
    client->send(msg);
    actualStateChange();
}

void Model::orderSend(const std::vector<FoodAmount> &foods) {
    if (!connected_) {
        qWarning() << __FUNCTION__ << "called while the connection is not alive!";
        return;
    }

    if (actualState != State::WaitingForOrder) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    for (const auto &item : foods) {
        auto search = std::find_if(availableFoods.begin(), availableFoods.end(),
                                   [id = item.FoodId](const FoodContains &other) { return id == other.FoodId; });
        if (search == availableFoods.end()) {
            qWarning() << __FUNCTION__ << "the ordered foods not in the foodlist:";
            return;
        }
    }
    client->send(QSharedPointer<OrderRequestMessage>::create(foods));
}

void Model::paySend() {
    if (!connected_) {
        qWarning() << __FUNCTION__ << "called while the connection is not alive!";
        return;
    }

    if (actualState != State::WaitingForPayIntent) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    if (!activeOrderId_) {
        qWarning() << __FUNCTION__ << "called without active order";
        return;
    }

    if (activeOrderStatus_ != OrderStatus::Completed) {
        qWarning() << __FUNCTION__ << "wrong order state";
        return;
    }

    auto msg = QSharedPointer<PayRequestMessage>::create(*activeOrderId_);
    client->send(msg);
}

void Model::exit() {
    exited();
}

void Model::on_connected() {
}

void Model::handleMessageArrived(QSharedPointer<Message> msg) {

    switch (msg->id()) {
    case MessageId::LoginReply:
        handleLoginReply(static_cast<const LoginReplyMessage &>(*msg));
        break;
    case MessageId::FoodListReply:
        handleListFoodReply(static_cast<const FoodListReplyMessage &>(*msg));
        break;
    case MessageId::OrderReply:
        handleOrderReply(static_cast<const OrderReplyMessage &>(*msg));
        break;
    case MessageId::PayReply:
        handlePayReply(static_cast<const PayReplyMessage &>(*msg));
        break;
    case MessageId::NotificationOrders:
        handleNotificationOrders(static_cast<const NotificationOrdersMessage &>(*msg));
        break;
    case MessageId::LoginRequest:
    case MessageId::FoodListRequest:
    case MessageId::OrderRequest:
    case MessageId::PayRequest:
    case MessageId::FoodChangeRequest:
    case MessageId::FoodChangeReply:
    case MessageId::CompleteFoodRequest:
    case MessageId::CompleteFoodReply:
    case MessageId::OrderArrivedRequest:
    case MessageId::OrderArrivedReply:
    case MessageId::OrderStatusChangeRequest:
    case MessageId::OrderStatusChangeReply:
        qWarning() << "Can't handle message:" << msg->id();
        break;
    }
}

void Model::handleLoginReply(const LoginReplyMessage &msg) {
    if (actualState != State::WaitingForLogin) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    switch (msg.Status) {
    case LoginStatus::Ok:
        actualStateChange();
        loginSucceded();
        break;
    case LoginStatus::Error:
        loginFailed();
        break;
    }
}

void Model::handleListFoodReply(const FoodListReplyMessage &msg) {
    availableFoods = msg.Foods;
    foodListRefreshed(availableFoods);
    if (actualState == State::WaitingForFoodList) {
        actualStateChange();
        readyToOrder();
    }
}

void Model::handleOrderReply(const OrderReplyMessage &msg) {
    if (actualState != State::WaitingForOrder) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    if (!msg.OrderedFoods.empty()) {
        actualStateChange(msg.OrderId);
        activeOrderStatus_ = OrderStatus::Pending;
        orderSucceded(msg.OrderedFoods);
    } else {
        orderFailed();
    }
}

void Model::handlePayReply(const PayReplyMessage &msg) {
    if (actualState != State::WaitingForPayIntent) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    switch (msg.Status) {
    case PayStatus::Success:
        actualStateChange();
        paySucceded();
        break;
    case PayStatus::Failed:
        payFailed();
        break;
    }
}

void Model::handleNotificationOrders(const NotificationOrdersMessage &msg) {
    if (actualState != State::WaitingForPayIntent) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    if (activeOrderId_ && *activeOrderId_ == msg.Order.OrderId) {
        activeOrderStatus_ = msg.Order.Status;
        statusChanged(msg.Order.Status);
    }
}

void Model::actualStateChange(std::optional<std::uint64_t> activeOrderId) {
    activeOrderId_ = activeOrderId;

    switch (actualState) {
    case State::WaitingForLogin:
        actualState = State::TitleScreen;
        activeOrderStatus_ = std::nullopt;
        break;
    case State::TitleScreen:
        actualState = State::WaitingForFoodList;
        activeOrderStatus_ = std::nullopt;
        break;
    case State::WaitingForFoodList:
        actualState = State::WaitingForOrder;
        activeOrderStatus_ = std::nullopt;
        break;
    case State::WaitingForOrder:
        actualState = State::WaitingForPayIntent;
        availableFoods.clear();
        break;
    case State::WaitingForPayIntent:
        actualState = State::TitleScreen;
        activeOrderStatus_ = std::nullopt;
        break;
    }
}

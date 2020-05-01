#include "Model.hpp"

QDebug &operator<<(QDebug &os, Model::State state) {
    switch (state) {
    case Model::State::WaitingForLogin:
        return os << "WaitingForLogin";
    case Model::State::Working:
        return os << "Working";
    }
    return os << "<INVALID>";
}

QDebug &operator<<(QDebug &os, MessageId id) {
    switch (id) {
    case MessageId::LoginRequest:
        return os << "LoginRequestMessage";
    case MessageId::ComplateFoodRequest:
        return os << "ComplateFoodRequestMessage";  
    case MessageId::FoodChangeRequest:
        return os << "FoodChangeRequestMessage";
    case MessageId::OrderStatusChangeRequest:
        return os << "OrderStatusChangeRequestMessage";
    }

    return os << "<INVALID>";
}

Model::Model(IClient *client, QObject *parent) : QObject(parent), client(client) {
    connect(client, &IClient::messageArrived, this, &Model::handleMessageArrived);
    connect(client, &IClient::connectionEstablished, this, [this] {
        connected_ = true;
        connectionStateChanged(connected_);
    });
    connect(client, &IClient::connectionLost, this, [this] {
        connected_ = false;
        connectionStateChanged(connected_);
    });
}

Model::~Model() = default;

void Model::connectToServer(const QString &host, quint32 port) {
    client->connectToHost(host, port);
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

    auto msg = QSharedPointer<LoginRequestMessage>::create(username.toStdString(), password.toStdString());

    client->send(msg);
}

void Model::complateListFoodRequest() {
    if (!connected_) {
        qWarning() << __FUNCTION__ << "called while the connection is not alive!";
        return;
    }

    if (actualState != State::Working) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    auto msg = QSharedPointer<ComplateFoodRequestMessage>::create();
    client->send(msg);
}

void Model::foodChangeRequest(const Delta &delta) {
    if (!connected_) {
        qWarning() << __FUNCTION__ << "called while the connection is not alive!";
        return;
    }

    if (actualState != State::Working) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    auto msg = QSharedPointer<FoodChangeRequestMessage>::create(delta);
    client->send(msg);
}

void Model::orderStatusChangeRequest(std::uint64_t orderId, OrderStatus status) {
    if (!connected_) {
        qWarning() << __FUNCTION__ << "called while the connection is not alive!";
        return;
    }

    if (actualState != State::Working) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    auto msg = QSharedPointer<OrderStatusChangeRequestMessage>::create(orderId, status);
    client->send(msg);
}

void Model::handleMessageArrived(QSharedPointer<Message> msg) {
    switch (msg->id()) {
    case MessageId::LoginReply:
        handleLoginReply(static_cast<const LoginReplyMessage &>(*msg));
        break;
    case MessageId::ComplateFoodReply:
        handleListFoodReply(static_cast<const ComplateFoodReplyMesage &>(*msg));
        break;
    case MessageId::FoodChangeReply:
        handleFoodChangeReply(static_cast<const FoodChangeReplyMessage &>(*msg));
        break;
    case MessageId::NotificationOrders:
        handleNotificationOrders(static_cast<const NotificationOrdersMessage &>(*msg));
        break;
    case MessageId::OrderStatusChangeReply:
        handleOrderStatusChangeReply(static_cast<const OrderStatusChangeReplyMessage &>(*msg));
        break;
    case MessageId::FoodListReply:
    case MessageId::OrderReply:
    case MessageId::PayReply:
    case MessageId::LoginRequest:
    case MessageId::FoodListRequest:
    case MessageId::OrderRequest:
    case MessageId::PayRequest:
    case MessageId::ComplateFoodRequest:
    case MessageId::FoodChangeRequest:
    case MessageId::OrderStatusChangeRequest:
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
        ActualStateChange(actualState);
        loginSucceded();
        break;
    case LoginStatus::Error:
        loginFailed();
        break;
    }
}

void Model::handleNotificationOrders(const NotificationOrdersMessage &msg) {
    if (actualState != State::Working) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }
    requestedOrders_.push_back(msg.Order);
    newOrderArrived(requestedOrders_);
}

void Model::handleOrderStatusChangeReply(const OrderStatusChangeReplyMessage &status) {
    if (actualState != State::Working) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    switch (status.Status) {
    case ReplyStatus::Success:
        orderStatusChangeSucceded(status.OrderId, status.NewStatus);
        break;
    case ReplyStatus::Failed:
        orderStatusChangeFailed(status.OrderId, status.NewStatus);
        break;
    }
}

void Model::handleListFoodReply(const ComplateFoodReplyMesage &msg) {
    if (actualState != State::Working) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    availableFoods_ = msg.Foods;
    foodListRefreshed(availableFoods_);
}

void Model::handleFoodChangeReply(const FoodChangeReplyMessage &msg) {
    if (actualState != State::Working) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    switch (msg.Status) {
    case FoodChangeStatus::Success:
        foodChangeSucceded();
        break;
    case FoodChangeStatus::Failed:
        foodChangeFailed();
        break;
    }
}
void Model::ActualStateChange(const State &state) {
    switch (state) {
    case State::WaitingForLogin:
        actualState = State::Working;
        break;
    }
}
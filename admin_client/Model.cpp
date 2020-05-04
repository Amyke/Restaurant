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
    case MessageId::CompleteFoodRequest:
        return os << "CompleteFoodRequestMessage";  
    case MessageId::FoodChangeRequest:
        return os << "FoodChangeRequestMessage";
    case MessageId::OrderStatusChangeRequest:
        return os << "OrderStatusChangeRequestMessage";
    case MessageId::OrderArrivedRequest:
        return os << "OrderedArrivedRequestMessage";
    }

    return os << "<INVALID>";
}

Model::Model(IClient *client, QObject *parent) : QObject(parent), client(client) {
    qRegisterMetaType<std::uint64_t>();

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

void Model::ordersListRequest(std::uint64_t fromDate, std::uint64_t toDate) {
    if (!connected_) {
        qWarning() << __FUNCTION__ << "called while the connection is not alive!";
        return;
    }

    if (actualState != State::Working) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    auto msg = QSharedPointer<OrderArrivedRequestMessage>::create(fromDate, toDate);
    client->send(msg);

}

void Model::completeListRequest() {
    if (!connected_) {
        qWarning() << __FUNCTION__ << "called while the connection is not alive!";
        return;
    }

    if (actualState != State::Working) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    auto msg = QSharedPointer<CompleteFoodRequestMessage>::create();
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
    case MessageId::CompleteFoodReply:
        handleListFoodReply(static_cast<const CompleteFoodReplyMesage &>(*msg));
        break;
    case MessageId::FoodChangeReply:
        handleFoodChangeReply(static_cast<const FoodChangeReplyMessage &>(*msg));
        break;
    case MessageId::OrderArrivedReply:
        handleOrderListReply(static_cast<const OrderArrivedReplyMessage &>(*msg));
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
    case MessageId::OrderArrivedRequest:
    case MessageId::FoodListRequest:
    case MessageId::OrderRequest:
    case MessageId::PayRequest:
    case MessageId::CompleteFoodRequest:
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

void Model::handleOrderListReply(const OrderArrivedReplyMessage &msg) {
    if (actualState != State::Working) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    orderListArrived(msg.Orders);
}


void Model::handleNotificationOrders(const NotificationOrdersMessage &msg) {
    if (actualState != State::Working) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }
    newOrderArrived(msg.Order);
}

void Model::handleOrderStatusChangeReply(const OrderStatusChangeReplyMessage &status) {
    if (actualState != State::Working) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    switch (status.Status) {
    case ReplyStatus::Success:
        orderStatusChangeSucceded(status.OrderId, status.NewStatus, status.Date);
        break;
    case ReplyStatus::Failed:
        orderStatusChangeFailed(status.OrderId, status.NewStatus, status.Date);
        break;
    }
}

void Model::handleListFoodReply(const CompleteFoodReplyMesage &msg) {
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
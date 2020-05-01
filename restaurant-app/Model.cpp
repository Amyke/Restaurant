#include "Model.hpp"

#include <QtCore/QDebug>

QDebug &operator<<(QDebug &os, Model::State state) {
    switch (state) {
    case Model::State::TitleScreen:
        return os << "TitleScreen";
    case Model::State::WaitingForLogin:
        return os << "WaitingForLogin";
    case Model::State::WaitingForOrder:
        return os << "WaitingForOrder";
    case Model::State::WaitingForPayIntent:
        return os << "WaitingForPayIntent";
    }
    return os << "<INVALID>";
}

QDebug &operator<<(QDebug &os, MessageId id) {
    switch (id) {
    case MessageId::LoginRequest:
        return os << "LoginRequestMessage";
    case MessageId::FoodListRequest:
        return os << "FoodListRequestMessage";
    case MessageId::OrderRequest:
        return os << "FoodListReplyMessage";
    case MessageId::PayRequest:
        return os << "PayRequestMessage";
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

    auto msg = QSharedPointer<PayRequestMessage>::create(true);
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
    case MessageId::LoginRequest:
    case MessageId::FoodListRequest:
    case MessageId::OrderRequest:
    case MessageId::PayRequest:
    case MessageId::FoodChangeRequest:
    case MessageId::FoodChangeReply:
    case MessageId::NotificationOrders:
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

void Model::handleListFoodReply(const FoodListReplyMessage &msg) {
    if (actualState != State::TitleScreen) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }
    
    ActualStateChange(actualState);
    availableFoods = msg.Foods;
    foodListRefreshed(availableFoods);
}

void Model::handleOrderReply(const OrderReplyMessage &msg) {
    if (actualState != State::WaitingForOrder) {
        qWarning() << __FUNCTION__ << "called in invalid state:" << actualState;
        return;
    }

    if (!msg.OrderedFoods.empty()) {
        ActualStateChange(actualState);
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
        ActualStateChange(actualState);
        paySucceded();
        break;
    case PayStatus::Failed:
        payFailed();
        break;
    }
}

void Model::ActualStateChange(const State &state) {

    switch (state) {
    case State::WaitingForLogin:
        actualState = State::TitleScreen;
        break;
    case State::TitleScreen:
        actualState = State::WaitingForOrder;
        break;
    case State::WaitingForOrder:
        actualState = State::WaitingForPayIntent;
        availableFoods.clear();
        break;
    case State::WaitingForPayIntent:
        actualState = State::TitleScreen;
        break;
    }
}

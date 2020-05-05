#include "Client.hpp"

#include "Message.hpp"

Client::Client(QObject *parent) : IClient(parent), socket_(new QTcpSocket(this)), protocol_(new Protocol(socket_)) {
    connect(socket_, &QTcpSocket::connected, this, &Client::onConnected);
    connect(socket_, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(protocol_, &Protocol::messageArrived, this, &Client::messageArrived);
    connect(socket_, qOverload<QTcpSocket::SocketError>(&QTcpSocket::error), [this](QTcpSocket::SocketError error) {
        if (error == QTcpSocket::SocketError::ConnectionRefusedError) {
            connectionLost();
        }
    });
    connect(socket_, &QTcpSocket::disconnected, this, &Client::connectionLost);
}

Client::~Client() = default;

void Client::connectToHost(const QString &hostname, quint16 port) {
    socket_->connectToHost(hostname, port);
}

void Client::send(MessagePtr msg) {
    switch (msg->id()) {
    case MessageId::LoginRequest:
        protocol_->sendMessage(static_cast<const LoginRequestMessage &>(*msg));
        break;
    case MessageId::LoginReply:
        protocol_->sendMessage(static_cast<const LoginReplyMessage &>(*msg));
        break;
    case MessageId::FoodListRequest:
        protocol_->sendMessage(static_cast<const FoodListRequestMessage &>(*msg));
        break;
    case MessageId::FoodListReply:
        protocol_->sendMessage(static_cast<const FoodListReplyMessage &>(*msg));
        break;
    case MessageId::OrderRequest:
        protocol_->sendMessage(static_cast<const OrderRequestMessage &>(*msg));
        break;
    case MessageId::OrderReply:
        protocol_->sendMessage(static_cast<const OrderReplyMessage &>(*msg));
        break;
    case MessageId::PayRequest:
        protocol_->sendMessage(static_cast<const PayRequestMessage &>(*msg));
        break;
    case MessageId::PayReply:
        protocol_->sendMessage(static_cast<const PayReplyMessage &>(*msg));
        break;
    case MessageId::FoodChangeReply:
        protocol_->sendMessage(static_cast<const FoodChangeReplyMessage &>(*msg));
        break;
    case MessageId::FoodChangeRequest:
        protocol_->sendMessage(static_cast<const FoodChangeRequestMessage &>(*msg));
        break;
    case MessageId::NotificationOrders:
        protocol_->sendMessage(static_cast<const NotificationOrdersMessage &>(*msg));
        break;
    case MessageId::OrderStatusChangeRequest:
        protocol_->sendMessage(static_cast<const OrderStatusChangeRequestMessage &>(*msg));
        break;
    case MessageId::OrderStatusChangeReply:
        protocol_->sendMessage(static_cast<const OrderStatusChangeReplyMessage &>(*msg));
        break;
    case MessageId::CompleteFoodRequest:
        protocol_->sendMessage(static_cast<const CompleteFoodRequestMessage &>(*msg));
        break;
    case MessageId::CompleteFoodReply:
        protocol_->sendMessage(static_cast<const CompleteFoodReplyMesage &>(*msg));
        break;
    case MessageId::OrderArrivedRequest:
        protocol_->sendMessage(static_cast<const OrderArrivedRequestMessage &>(*msg));
        break;
    case MessageId::OrderArrivedReply:
        protocol_->sendMessage(static_cast<const OrderArrivedReplyMessage &>(*msg));
        break;
    }
}

void Client::close() {
    socket_->close();
}

void Client::onConnected() {
    qDebug() << "Connected, yay!!";
    connectionEstablished();
}

void Client::onReadyRead() {
    protocol_->readMessages();
}

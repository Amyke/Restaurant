#include "Protocol.hpp"

#include <QIODevice>

template <typename T>
QSharedPointer<T> msgpackDeserializePtr(const msgpack::object &obj) {
    auto ptr = QSharedPointer<T>(new T);
    ptr->msgpack_unpack(obj);
    return ptr;
}

QSharedPointer<Message> decodeMessage(MessageId type, const msgpack::object &obj) {
    switch (type) {
    case MessageId::LoginRequest:
        return msgpackDeserializePtr<LoginRequestMessage>(obj);
    case MessageId::LoginReply:
        return msgpackDeserializePtr<LoginReplyMessage>(obj);
    case MessageId::FoodListRequest:
        return msgpackDeserializePtr<FoodListRequestMessage>(obj);
    case MessageId::FoodListReply:
        return msgpackDeserializePtr<FoodListReplyMessage>(obj);
    case MessageId::OrderRequest:
        return msgpackDeserializePtr<OrderRequestMessage>(obj);
    case MessageId::OrderReply:
        return msgpackDeserializePtr<OrderReplyMessage>(obj);
    case MessageId::PayRequest:
        return msgpackDeserializePtr<PayRequestMessage>(obj);
    case MessageId::PayReply:
        return msgpackDeserializePtr<PayReplyMessage>(obj);
    case MessageId::FoodChangeRequest:
        return msgpackDeserializePtr<FoodChangeRequestMessage>(obj);
    case MessageId::FoodChangeReply:
        return msgpackDeserializePtr<FoodChangeReplyMessage>(obj);
    case MessageId::OrderArrivedRequest:
        return msgpackDeserializePtr<OrderArrivedRequestMessage>(obj);
    case MessageId::OrderArrivedReply:
        return msgpackDeserializePtr<OrderArrivedReplyMessage>(obj);
    case MessageId::NotificationOrders:
        return msgpackDeserializePtr<NotificationOrdersMessage>(obj);
    case MessageId::CompleteFoodRequest:
        return msgpackDeserializePtr<CompleteFoodRequestMessage>(obj);
    case MessageId::CompleteFoodReply:
        return msgpackDeserializePtr<CompleteFoodReplyMesage>(obj);
    case MessageId::OrderStatusChangeRequest:
        return msgpackDeserializePtr<OrderStatusChangeRequestMessage>(obj);
    case MessageId::OrderStatusChangeReply:
        return msgpackDeserializePtr<OrderStatusChangeReplyMessage>(obj);
    }
    return nullptr;
};

Protocol::Protocol(QIODevice *device) : QObject(device), device_(device) {
    qRegisterMetaType<MessagePtr>();
}

void Protocol::readMessages() {
    readBuffer_.append(device_->readAll());
    std::size_t offset = 0;
    try {
        while (true) {
            msgpack::object_handle oh;
            if (!lastHeader_) {
                std::size_t headerOffset = offset;
                oh = msgpack::unpack(readBuffer_.data(), readBuffer_.size(), headerOffset);
                if (oh->is_nil()) {
                    break;
                }
                offset = headerOffset;
                MessageHeader header;
                header.msgpack_unpack(*oh);
                lastHeader_ = header;
            }

            std::size_t messageOffset = offset;
            oh = msgpack::unpack(readBuffer_.data(), readBuffer_.size(), messageOffset);
            if (oh->is_nil()) {
                break;
            }
            offset = messageOffset;
            auto message = decodeMessage(lastHeader_->id, *oh);
            lastHeader_ = std::nullopt;
            if (message != nullptr) {
                messageArrived(message);
            }
        }
    } catch (const msgpack::insufficient_bytes &) {
    }
    readBuffer_ = readBuffer_.mid(offset);
}

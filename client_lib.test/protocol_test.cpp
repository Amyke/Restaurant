#include "Protocol.hpp"
#include <QtTest/QtTest>

bool operator==(const LoginRequestMessage &lhs, const LoginRequestMessage &rhs) {
    return lhs.Name == rhs.Name && lhs.Password == rhs.Password;
}
bool operator==(const LoginReplyMessage &lhs, const LoginReplyMessage &rhs) {
    return lhs.Status == rhs.Status;
}
bool operator==(const PayReplyMessage &lhs, const PayReplyMessage &rhs) {
    return lhs.Status == rhs.Status;
}
    /*bool operator==(const FoodListRequestMessage &lhs, const FoodListRequestMessage &rhs) {
    return lhs.Foods.data == rhs.Foods.data;
}
bool operator==(const OrderRequestMessage &lhs, const OrderRequestMessage &rhs) {
    return lhs.OrderedFoods.data == rhs.OrderedFoods.data;
}*/

class ProtocolTest : public QObject {
    Q_OBJECT

private slots:
    void init() {
        buffer_ = new QBuffer;
        buffer_->open(QIODevice::ReadWrite);
        protocol_ = new Protocol(buffer_);
    }

    void cleanup() {
        delete protocol_;
        delete buffer_;
    }

    void SendOneMessage() {
        LoginRequestMessage msg("name", "password");
        protocol_->sendMessage(msg);
        QVERIFY(!buffer_->data().isEmpty());
        buffer_->seek(0);
        auto messages = receiveMessages();
        QCOMPARE(messages.size(), 1);
        auto actual = messages.front();
        QCOMPARE(actual->id(), MessageId::LoginRequest);
        QCOMPARE(static_cast<const LoginRequestMessage &>(*actual), msg);
    }

    void SendMoreMessage() {
        LoginRequestMessage msg1("ûû", "12c");
        LoginRequestMessage msg2("בבב", "11c");
        protocol_->sendMessage(msg1);
        protocol_->sendMessage(msg2);
        buffer_->seek(0);
        auto message = receiveMessages();
        QCOMPARE(message.size(), 2);
        auto actual1 = message.front();
        auto actual2 = message.back();
        QCOMPARE(actual1->id(), MessageId::LoginRequest);
        QCOMPARE(static_cast<const LoginRequestMessage &>(*actual1), msg1);
        QCOMPARE(actual2->id(), MessageId::LoginRequest);
        QCOMPARE(static_cast<const LoginRequestMessage &>(*actual2), msg2); 
    }
    void InterleavedMessages() {
        LoginRequestMessage msg12("admin", "admin");
        LoginReplyMessage msg22(LoginStatus::Ok);
        PayReplyMessage msg(PayStatus::Failed);

        protocol_->sendMessage(msg12);
        buffer_->seek(0);
        auto message = receiveMessages();

        QCOMPARE(message.size(), 1);
        auto actual = message.front();
        
        QCOMPARE(actual->id(), MessageId::LoginRequest);
        QCOMPARE(static_cast<const LoginRequestMessage &>(*actual), msg12);
        
        auto pos = buffer_->pos();
        protocol_->sendMessage(msg22);
        buffer_->seek(pos);
        message = receiveMessages();
        
        QCOMPARE(message.size(), 1);
        actual = message.front();

        QCOMPARE(actual->id(), MessageId::LoginReply);
        QCOMPARE(static_cast<const LoginReplyMessage&>(*actual), msg22);

        pos = buffer_->pos();
        protocol_->sendMessage(msg);
        buffer_->seek(pos);
        message = receiveMessages();

        QCOMPARE(message.size(), 1);
        actual = message.front();
        
        QCOMPARE(actual->id(), MessageId::PayReply);
        QCOMPARE(static_cast<const PayReplyMessage &>(*actual), msg);
    }

private:
    std::vector<MessagePtr> receiveMessages() {
        QSignalSpy watcher(protocol_, &Protocol::messageArrived);
        protocol_->readMessages();

        std::vector<MessagePtr> messages;
        for (const auto &params : watcher) {
            const auto &p = params.front();
            messages.emplace_back(p.value<MessagePtr>());
        }
        return messages;
    }

    QBuffer* buffer_;
    Protocol *protocol_;
};

QTEST_GUILESS_MAIN(ProtocolTest);
#include "protocol_test.moc"

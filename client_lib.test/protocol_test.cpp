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

Q_DECLARE_METATYPE(QSharedPointer<CompleteFoodReplyMesage>);

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
        LoginRequestMessage msg("name", "password", true);
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
        LoginRequestMessage msg1("űű", "12c", true);
        LoginRequestMessage msg2("ááá", "11c", true);
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
        LoginRequestMessage msg12("admin", "admin", true);
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
        QCOMPARE(static_cast<const LoginReplyMessage &>(*actual), msg22);

        pos = buffer_->pos();
        protocol_->sendMessage(msg);
        buffer_->seek(pos);
        message = receiveMessages();

        QCOMPARE(message.size(), 1);
        actual = message.front();

        QCOMPARE(actual->id(), MessageId::PayReply);
        QCOMPARE(static_cast<const PayReplyMessage &>(*actual), msg);
    }

    void partialMessage_data() {
        auto msg = QSharedPointer<CompleteFoodReplyMesage>::create(std::vector<Food>{{{0, "Name", 500, 10}, true}});

        QTest::addColumn<QSharedPointer<CompleteFoodReplyMesage>>("msg");
        QTest::addColumn<int>("offset");

        msgpack::sbuffer buffer;
        msgpack::pack(buffer, *msg);

        for (int i = 0; i < buffer.size(); ++i) {
            QTest::addRow("%d", i) << msg << i;
        }
    }

    void partialMessage() {
        QFETCH(QSharedPointer<CompleteFoodReplyMesage>, msg);
        QFETCH(int, offset);

        msgpack::pack(*buffer_, MessageHeader{msg->id()});

        msgpack::sbuffer buffer;
        msgpack::pack(buffer, *msg);
        buffer_->write(buffer.data(), offset);
        buffer_->seek(0);

        QCOMPARE(receiveMessages().size(), 0);

        int pos = buffer_->pos();
        buffer_->write(buffer.data() + offset, buffer.size() - offset);
        buffer_->seek(pos);
        auto received = receiveMessages();
        QCOMPARE(received.size(), 1);
        QCOMPARE(received[0]->id(), msg->id());
    }

    /*void msgpackRegression() {
        unsigned char buffer[] = {145, 12,  145, 9,   145, 148, 146, 148, 1,   170, 80,  97,  108, 97,  99,  115,
                                  105, 110, 116, 97,  100, 10,  195, 146, 148, 2,   173, 82,  195, 161, 110, 116,
                                  111, 116, 116, 32,  115, 97,  106, 116, 205, 4,   176, 5,   195, 146, 148, 3,
                                  168, 66,  111, 108, 111, 103, 110, 97,  105, 205, 5,   120, 10,  195, 146, 148,
                                  4,   169, 67,  97,  109, 97,  109, 98,  101, 114, 116, 205, 6,   64,  4,   194};

        buffer_->write(reinterpret_cast<char*>(buffer), sizeof buffer);
        buffer_->seek(0);

        receiveMessages();
    }*/

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

    QBuffer *buffer_;
    Protocol *protocol_;
};

QTEST_GUILESS_MAIN(ProtocolTest);
#include "protocol_test.moc"

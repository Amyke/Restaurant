#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include "Message.hpp"
#include <QObject>
#include <QSharedPointer>
#include <memory>
#include <optional>

struct Message;
class QIODevice;

using MessagePtr = QSharedPointer<Message>;
Q_DECLARE_METATYPE(MessagePtr);

class Protocol : public QObject {
    Q_OBJECT
public:
    explicit Protocol(QIODevice *device);

    template <typename T>
    void sendMessage(const T &msg);

    void readMessages();

signals:
    void messageArrived(MessagePtr msg);

private:
    QIODevice *device_;
    QByteArray readBuffer_;

    std::optional<MessageHeader> lastHeader_;
};

template <typename T>
void Protocol::sendMessage(const T &msg) {
    static_assert(std::is_base_of_v<Message, T>, "Not message type");

    msgpack::pack(*device_, MessageHeader{msg.id()});
    msgpack::pack(*device_, msg);
}

#endif //! PROTOCOL_HPP
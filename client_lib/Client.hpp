#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <memory>
#include <optional>

#include <QtNetwork/QTcpSocket>

#include "Message.hpp"
#include "Protocol.hpp"
#include <string>

class IClient : public QObject {
    Q_OBJECT
public:
    using QObject::QObject;

    virtual void connectToHost(const QString &hostname, quint16 port) = 0;

    virtual void send(MessagePtr msg) = 0;
    virtual void close() = 0;

signals:
    void connectionEstablished();
    void connectionLost();

    void messageArrived(QSharedPointer<Message> msg);
};

class Client : public IClient {
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    ~Client();

    void connectToHost(const QString &hostname, quint16 port) override;

    void send(MessagePtr msg) override;
    void close() override;

private:
    void onConnected();
    void onReadyRead();

    QTcpSocket *socket_;
    Protocol *protocol_;
};

#endif //! CLIENT_HPP

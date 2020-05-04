#include "Model.hpp"
#include <QtTest/QtTest>

class FakeClient : public IClient {
    Q_OBJECT
public:
    FakeClient() : messageSentSpy(this, &FakeClient::messageSent) {
    }

    void emitConnectionEstablished() {
        connectionEstablished();
    }

    void emitConnectionLost() {
        connectionLost();
    }

    void emitMessageArrived(MessagePtr msg) {
        messageArrived(msg);
    }

    void connectToHost(const QString &hostname, quint16 port) override {
    }

    void send(MessagePtr msg) override {
        messageSent(msg);
    }

    void close() override {
    }

    QSignalSpy messageSentSpy;

signals:
    void messageSent(MessagePtr msg);
};

Q_DECLARE_METATYPE(std::vector<Orders>);
Q_DECLARE_METATYPE(std::vector<Food>);
Q_DECLARE_METATYPE(OrderStatus);
Q_DECLARE_METATYPE(ReplyStatus);

class ModelTest : public QObject {
    Q_OBJECT
private slots:
    void initTestCase() {
        qRegisterMetaType<std::vector<Orders>>();
        qRegisterMetaType<std::vector<Food>>();
        qRegisterMetaType<OrderStatus>();
        qRegisterMetaType<ReplyStatus>();
    }

    void init() {
        client_ = new FakeClient;
        model_ = new Model(client_);
    }
    void cleanup() {
        delete model_;
        delete client_;
    }

    void initiallyThereIsNoConnection() {
        // Assert
        QCOMPARE(model_->isConnected(), false);
    }

    void SuccessConnected() {
        // Act
        client_->emitConnectionEstablished();

        // Assert
        auto connection = model_->isConnected();
        QCOMPARE(connection, true);
    }
    void connectionLost() {
        // Arrange
        Given_ConnectedToServer();

        // Act
        client_->emitConnectionLost();

        // Assert
        auto connection = model_->isConnected();
        QCOMPARE(connection, false);
    }

    void loginInformationSentCorrectly() {
        // Arrange
        Given_ConnectedToServer();
        // Act
        model_->login("admin", "admin");

        // Assert
        auto params = client_->messageSentSpy.front();
        auto msg = params.front().value<MessagePtr>();

        QCOMPARE(msg->id(), MessageId::LoginRequest);

        auto request = msg.staticCast<LoginRequestMessage>();
        QCOMPARE(request->Name, "admin");
        QCOMPARE(request->Password, "admin");
    }

    void successfulLogin() {
        // Arrange
        Given_ConnectedToServer();
        model_->login("admin", "admin");
        QSignalSpy loginSuccededSpy(model_, &Model::loginSucceded);
        QSignalSpy loginFailedSpy(model_, &Model::loginFailed);

        // Act
        MessagePtr reply(new LoginReplyMessage(LoginStatus::Ok));
        client_->emitMessageArrived(reply);

        // Assert
        QCOMPARE(loginSuccededSpy.size(), 1);
        QCOMPARE(loginFailedSpy.size(), 0);
    }

    void RequestedOrderListSent() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();

        client_->messageSentSpy.clear();

        auto now = QDateTime::currentDateTime();
        auto oneWeekAgo = now.addDays(-7);

        // Act
        model_->ordersListRequest(now.toTime_t(), oneWeekAgo.toTime_t());

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 1);
        auto params = client_->messageSentSpy.front();
        auto msg = params.front().value<MessagePtr>();

        QCOMPARE(msg->id(), MessageId::OrderArrivedRequest);
    }

    void RequestOrderListSuccessful() {
        // Arrangd
        Given_ConnectedToServer();
        Given_IAmLoggedIn();

        auto now = QDateTime::currentDateTime();
        auto oneWeekAgo = now.addDays(-7);

        model_->ordersListRequest(now.toTime_t(), oneWeekAgo.toTime_t());
        QSignalSpy orderListRefreshedSpy(model_, &Model::orderListArrived);

        // Act
        client_->emitMessageArrived(QSharedPointer<OrderArrivedReplyMessage>::create(std::vector<Orders>{}));

        // Assert
        QCOMPARE(orderListRefreshedSpy.size(), 1);
    }
    void CompleteFoodListRequestSent() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();

        client_->messageSentSpy.clear();

        // Act
        model_->completeListRequest();

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 1);
        auto params = client_->messageSentSpy.front();
        auto msg = params.front().value<MessagePtr>();

        QCOMPARE(msg->id(), MessageId::CompleteFoodRequest);
    }

    void CompleteFoodListSuccessful() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();

        model_->completeListRequest();
        QSignalSpy foodListRefreshedSpy(model_, &Model::foodListRefreshed);

        // Act
        client_->emitMessageArrived(QSharedPointer<CompleteFoodReplyMesage>::create(std::vector<Food>{}));

        // Assert
        QCOMPARE(foodListRefreshedSpy.size(), 1);
    }

    void NotificationOrdersArrivedSuccess() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();
        QSignalSpy orderArrivedSpy(model_, &Model::newOrderArrived);

        Orders order = {1, "table", {}, QDateTime::currentDateTime().toTime_t(), OrderStatus::Pending};
        // Act
        client_->emitMessageArrived(QSharedPointer<NotificationOrdersMessage>::create(order));

        // Assert
        QCOMPARE(orderArrivedSpy.size(), 1);
    }

    void orderStatusChangeRequestSent() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();

        client_->messageSentSpy.clear();

        // Act
        model_->orderStatusChangeRequest(2, OrderStatus::InProgress);

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 1);
        auto params = client_->messageSentSpy.front();
        auto msg = params.front().value<MessagePtr>();

        QCOMPARE(msg->id(), MessageId::OrderStatusChangeRequest);
    }

    void orderStatusChangeRequestSuccessful() {

        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();

        model_->orderStatusChangeRequest(2, OrderStatus::InProgress);
        QSignalSpy orderStatusSuccessChangeSpy(model_, &Model::orderStatusChangeSucceded);
        QSignalSpy orderStatusChangeFailedSpy(model_, &Model::orderStatusChangeFailed);

        // Act
        client_->emitMessageArrived(QSharedPointer<OrderStatusChangeReplyMessage>::create(
            1, ReplyStatus::Success, OrderStatus::InProgress, QDateTime::currentDateTime().toTime_t()));

        // Assert
        QCOMPARE(orderStatusSuccessChangeSpy.size(), 1);
        QCOMPARE(orderStatusChangeFailedSpy.size(), 0);
    }
    void foodChangeRequestSent() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();

        client_->messageSentSpy.clear();

        // Act
        model_->foodChangeRequest(Delta{{}, {}});

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 1);
        auto params = client_->messageSentSpy.front();
        auto msg = params.front().value<MessagePtr>();

        QCOMPARE(msg->id(), MessageId::FoodChangeRequest);
    }

    void foodChangeRequestSuccessful() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();

        model_->foodChangeRequest(Delta{{}, {}});
        QSignalSpy foodChangeSuccesSpy(model_, &Model::foodChangeSucceded);
        QSignalSpy foodChangeFailedSpy(model_, &Model::foodChangeFailed);

        // Act
        auto now = QDateTime::currentDateTime();
        client_->emitMessageArrived(QSharedPointer<FoodChangeReplyMessage>::create(FoodChangeStatus::Success));

        // Assert
        QCOMPARE(foodChangeSuccesSpy.size(), 1);
        QCOMPARE(foodChangeFailedSpy.size(), 0);
    }

    // Failed Case:

    void loginWithOutConnection() {
        // Act
        model_->login("admin", "admin");

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }

    void failedLogin() {
        // Arrange
        Given_ConnectedToServer();
        model_->login("admin", "admin");
        QSignalSpy loginSuccededSpy(model_, &Model::loginSucceded);
        QSignalSpy loginFailedSpy(model_, &Model::loginFailed);

        // Act
        MessagePtr reply(new LoginReplyMessage(LoginStatus::Error));
        client_->emitMessageArrived(reply);

        // Assert
        QCOMPARE(loginFailedSpy.size(), 1);
        QCOMPARE(loginSuccededSpy.size(), 0);
    }
    void orderStatusChangeRequestFailed() {

        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();

        model_->orderStatusChangeRequest(2, OrderStatus::InProgress);
        QSignalSpy orderStatusSuccessChangeSpy(model_, &Model::orderStatusChangeSucceded);
        QSignalSpy orderStatusChangeFailedSpy(model_, &Model::orderStatusChangeFailed);

        // Act
        auto now = QDateTime::currentDateTime();
        client_->emitMessageArrived(QSharedPointer<OrderStatusChangeReplyMessage>::create(
            1, ReplyStatus::Failed, OrderStatus::InProgress, now.toTime_t()));

        // Assert
        QCOMPARE(orderStatusChangeFailedSpy.size(), 1);
        QCOMPARE(orderStatusSuccessChangeSpy.size(), 0);
    }

    void orderStatusChangeRequestWithoutConnection() {
        // Arrange
        Given_IAmLoggedIn();

        // Act
        model_->orderStatusChangeRequest(2, OrderStatus::InProgress);

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }

    void orderStatusChangeRequestWithoutLogin() {
        // Arrange
        Given_ConnectedToServer();

        // Act
        model_->orderStatusChangeRequest(2, OrderStatus::InProgress);

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }

    void foodChangeRequestFailed() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();

        model_->foodChangeRequest(Delta{{}, {}});
        QSignalSpy foodChangeSuccesSpy(model_, &Model::foodChangeSucceded);
        QSignalSpy foodChangeFailedSpy(model_, &Model::foodChangeFailed);

        // Act
        client_->emitMessageArrived(QSharedPointer<FoodChangeReplyMessage>::create(FoodChangeStatus::Failed));

        // Assert
        QCOMPARE(foodChangeFailedSpy.size(), 1);
        QCOMPARE(foodChangeSuccesSpy.size(), 0);
    }
    void foodChangeRequestWithoutConnection() {
        // Arrange
        Given_IAmLoggedIn();

        // Act
        model_->foodChangeRequest(Delta{{}, {}});

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }
    void foodChangeRequestWithoutLogin() {
        // Arrange
        Given_ConnectedToServer();

        // Act
        model_->foodChangeRequest(Delta{{}, {}});

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }

private:
    void Given_ConnectedToServer() {
        client_->emitConnectionEstablished();
    }

    void Given_IAmLoggedIn(QString username = "admin", QString password = "admin") {
        model_->login(username, password);
        client_->emitMessageArrived(QSharedPointer<LoginReplyMessage>::create(LoginStatus::Ok));
    }

    Model *model_;
    FakeClient *client_;
};

QTEST_GUILESS_MAIN(ModelTest);
#include "model_test.moc"
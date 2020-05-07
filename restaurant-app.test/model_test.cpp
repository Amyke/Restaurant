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

Q_DECLARE_METATYPE(std::vector<FoodContains>);
class ModelTest : public QObject {
    Q_OBJECT
private slots:
    void initTestCase() {
        qRegisterMetaType<std::vector<FoodContains>>();
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
        model_->login("elte", "ik");

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 1);
        auto params = client_->messageSentSpy.front();
        auto msg = params.front().value<MessagePtr>();

        QCOMPARE(msg->id(), MessageId::LoginRequest);

        auto request = msg.staticCast<LoginRequestMessage>();
        QCOMPARE(request->Name, "elte");
        QCOMPARE(request->Password, "ik");
    }

    void successfulLogin() {
        // Arrange
        Given_ConnectedToServer();
        model_->login("elte", "ik");
        QSignalSpy loginSuccededSpy(model_, &Model::loginSucceded);
        QSignalSpy loginFailedSpy(model_, &Model::loginFailed);

        // Act
        MessagePtr reply(new LoginReplyMessage(LoginStatus::Ok));
        client_->emitMessageArrived(reply);

        // Assert
        QCOMPARE(loginSuccededSpy.size(), 1);
        QCOMPARE(loginFailedSpy.size(), 0);
    }
    void listFoodRequestSent() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();

        client_->messageSentSpy.clear();

        // Act
        model_->listFoodRequest();

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 1);
        auto params = client_->messageSentSpy.front();
        auto msg = params.front().value<MessagePtr>();

        QCOMPARE(msg->id(), MessageId::FoodListRequest);
    }

    void listFoodRequestSuccessful() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();

        model_->listFoodRequest();
        QSignalSpy foodListRefreshedSpy(model_, &Model::foodListRefreshed);

        // Act
        client_->emitMessageArrived(QSharedPointer<FoodListReplyMessage>::create(std::vector<FoodContains>{}));

        // Assert
        QCOMPARE(foodListRefreshedSpy.size(), 1);
    }
    void orderRequestSent() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();
        Given_IHaveTheFoodList();

        client_->messageSentSpy.clear();

        // Act
        model_->orderSend(std::vector<FoodAmount>{});

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 1);

        auto params = client_->messageSentSpy.front();
        auto msg = params.front().value<MessagePtr>();

        QCOMPARE(msg->id(), MessageId::OrderRequest);
    }

    void orderRequestSucceded() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();
        Given_IHaveTheFoodList();

        model_->orderSend(std::vector<FoodAmount>{});
        QSignalSpy orderedSuccededSpy(model_, &Model::orderSucceded);
        QSignalSpy orderedFailedSpy(model_, &Model::orderFailed);
        std::uint64_t expectedOrderId = 42;

        // Act
        auto orderedFoods = std::vector<FoodContains>();
        orderedFoods.push_back({32, "Gulyas", 7660, 2});

        client_->emitMessageArrived(QSharedPointer<OrderReplyMessage>::create(expectedOrderId, orderedFoods));

        // Assert
        QCOMPARE(orderedSuccededSpy.size(), 1);
        auto actualFoods = orderedSuccededSpy[0][0].value<std::vector<FoodContains>>();
        // TODO
        // QCOMPARE(actualFoods, orderedFoods);
        QCOMPARE(orderedFailedSpy.size(), 0);
    }
    void payRequestSent() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();
        Given_IHaveTheFoodList();

        Given_IOrderedFoods();
        client_->messageSentSpy.clear();

        // Act
        model_->paySend();

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 1);

        auto params = client_->messageSentSpy.front();
        auto msg = params.front().value<MessagePtr>();

        QCOMPARE(msg->id(), MessageId::PayRequest);
    }
    void payRequestSucceded() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();
        Given_IHaveTheFoodList();

        Given_IOrderedFoods();

        model_->paySend();
        QSignalSpy paySuccededSpy(model_, &Model::paySucceded);
        QSignalSpy payFailedSpy(model_, &Model::payFailed);

        // Act
        client_->emitMessageArrived(QSharedPointer<PayReplyMessage>::create(PayStatus::Success));

        // Assert
        QCOMPARE(paySuccededSpy.size(), 1);
        QCOMPARE(payFailedSpy.size(), 0);
    }

    void listFoodRequestSentWhileIPayed() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();
        Given_IHaveTheFoodList();
        Given_IOrderedFoods();
        Given_IPayed();
        client_->messageSentSpy.clear();

        // Act
        model_->listFoodRequest();

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 1);
    }
    void listFoodRequestSuccededWhileIPayed() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();
        Given_IHaveTheFoodList();
        Given_IOrderedFoods();
        Given_IPayed();

        model_->listFoodRequest();
        QSignalSpy foodListRefreshedSpy(model_, &Model::foodListRefreshed);

        // Act
        client_->emitMessageArrived(QSharedPointer<FoodListReplyMessage>::create(std::vector<FoodContains>{}));

        // Assert
        QCOMPARE(foodListRefreshedSpy.size(), 1);
    }
    void exitSuccesful() {
        QSKIP("TODO");
        // Arrange
        model_->exit();
        QSignalSpy exitedSpy(model_, &Model::exited);

        // Act
    }

    // Something failed:
    void loginWithOutConnection() {
        // Act
        model_->login("elte", "ik");

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }

    void failedLogin() {
        // Arrange
        Given_ConnectedToServer();
        model_->login("elte", "ik");
        QSignalSpy loginSuccededSpy(model_, &Model::loginSucceded);
        QSignalSpy loginFailedSpy(model_, &Model::loginFailed);

        // Act
        MessagePtr reply(new LoginReplyMessage(LoginStatus::Error));
        client_->emitMessageArrived(reply);

        // Assert
        QCOMPARE(loginFailedSpy.size(), 1);
        QCOMPARE(loginSuccededSpy.size(), 0);
    }
    void OrderRequestFailedBecauseOfTheFoodList() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();
        Given_IHaveTheFoodList({});
        client_->messageSentSpy.clear();

        // Act
        model_->orderSend({{32, 4}});

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }

    void orderRequestFailedBecauseOfServer() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();
        Given_IHaveTheFoodList();

        model_->orderSend(std::vector<FoodAmount>{});
        QSignalSpy orderedSuccededSpy(model_, &Model::orderSucceded);
        QSignalSpy orderedFailedSpy(model_, &Model::orderFailed);

        // Act
        client_->emitMessageArrived(QSharedPointer<OrderReplyMessage>::create(0, std::vector<FoodContains>{}));

        // Assert
        QCOMPARE(orderedFailedSpy.size(), 1);
        QCOMPARE(orderedSuccededSpy.size(), 0);
    }

    void payRequestFailed() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();
        Given_IHaveTheFoodList();

        Given_IOrderedFoods();

        QSignalSpy paySuccededSpy(model_, &Model::paySucceded);
        QSignalSpy payFailedSpy(model_, &Model::payFailed);

        model_->paySend();

        // Act
        client_->emitMessageArrived(QSharedPointer<PayReplyMessage>::create(PayStatus::Failed));

        // Assert
        QCOMPARE(payFailedSpy.size(), 1);
        QCOMPARE(paySuccededSpy.size(), 0);
    }
    void ListFoodRequestWithOutConnection() {
        // Act
        model_->listFoodRequest();

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }

    void listFoodRequestWithoutLogin() {
        // Act
        Given_ConnectedToServer();
        model_->listFoodRequest();

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }
    void orderRequestWithoutConnection() {
        // Arrange
        Given_IHaveTheFoodList();
        client_->messageSentSpy.clear();

        // Act
        model_->orderSend(std::vector<FoodAmount>{});

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }

    void orderRequestWithoutLogin() {
        // Arrange
        Given_ConnectedToServer();
        Given_IHaveTheFoodList();
        client_->messageSentSpy.clear();

        // Act
        model_->orderSend(std::vector<FoodAmount>{});

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }
    void payRequestWithoutConnection() {
        // Arrange
        Given_IHaveTheFoodList();
        Given_IOrderedFoods();
        client_->messageSentSpy.clear();

        // Act
        model_->paySend();

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }

    void payRequestWithoutLogin() {
        // Arrange
        Given_ConnectedToServer();
        Given_IHaveTheFoodList();
        Given_IOrderedFoods();
        client_->messageSentSpy.clear();

        // Act
        model_->paySend();

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }

    void orderRequestWithoutFoodList() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();
        client_->messageSentSpy.clear();

        // Act
        model_->orderSend(std::vector<FoodAmount>{});

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }
    void payRequestWithoutFoodList() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();
        client_->messageSentSpy.clear();

        Given_IOrderedFoods();

        // Act
        model_->paySend();

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }
    void payRequestWithoutOrder() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();
        Given_IHaveTheFoodList();
        client_->messageSentSpy.clear();

        // Act
        model_->paySend();

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }
    void loginInWhileIHaveTheFoodList() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();
        Given_IHaveTheFoodList();
        client_->messageSentSpy.clear();

        // Act
        model_->login("admin", "admin");

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }
    void loginInWhileIOrderedFoods() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();
        Given_IHaveTheFoodList();
        Given_IOrderedFoods();
        client_->messageSentSpy.clear();

        // Act
        model_->login("admin", "admin");

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }
    void loginInWhileIPayed() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();
        Given_IHaveTheFoodList();
        Given_IOrderedFoods();
        Given_IPayed();
        client_->messageSentSpy.clear();

        // Act
        model_->login("admin", "admin");

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }
    void listFoodRequestWhileIOrdered() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();
        Given_IHaveTheFoodList();
        Given_IOrderedFoods();
        client_->messageSentSpy.clear();

        // Act
        model_->listFoodRequest();

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }
    void orderRequestWhileIPayed() {
        // Arrange
        Given_ConnectedToServer();
        Given_IAmLoggedIn();
        Given_IHaveTheFoodList();
        Given_IOrderedFoods();
        Given_IPayed();
        client_->messageSentSpy.clear();

        // Act
        model_->orderSend(std::vector<FoodAmount>{});

        // Assert
        QCOMPARE(client_->messageSentSpy.size(), 0);
    }

private:
    void Given_ConnectedToServer() {
        client_->emitConnectionEstablished();
    }

    void Given_IAmLoggedIn(QString username = "elte", QString password = "ik") {
        model_->login(username, password);
        client_->emitMessageArrived(QSharedPointer<LoginReplyMessage>::create(LoginStatus::Ok));
    }

    void Given_IHaveTheFoodList(std::vector<FoodContains> foods = {{32, "Gulyas", 7660, 2}}) {
        model_->listFoodRequest();
        client_->emitMessageArrived(QSharedPointer<FoodListReplyMessage>::create(foods));
    }
    void Given_IOrderedFoods(std::vector<FoodAmount> foods = {{32, 2}}) {
        model_->orderSend(foods);
        client_->emitMessageArrived(
            QSharedPointer<OrderReplyMessage>::create(42, std::vector<FoodContains>{{32, "Gulyas", 7660, 2}}));
    }
    void Given_IPayed() {
        model_->paySend();
        client_->emitMessageArrived(QSharedPointer<PayReplyMessage>::create(PayStatus::Success));
    }

    Model *model_;
    FakeClient *client_;
};

QTEST_GUILESS_MAIN(ModelTest);
#include "model_test.moc"

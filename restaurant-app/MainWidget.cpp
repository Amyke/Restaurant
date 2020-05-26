#include "MainWidget.hpp"

#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>

#include <QSettings>

#include <QtCore/QAbstractTableModel>

#include "EndWidget.hpp"
#include "LoginWidget.hpp"
#include "Model.hpp"
#include "OrderWidget.hpp"
#include "PayWidget.hpp"
#include "WelcomeWidget.hpp"

MainWidget::MainWidget(QWidget *parent) : QStackedWidget(parent), model(new Model(new Client(this), this)) {

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Restaurant", "menu");
    auto hostName = settings.value("serverHost", "localhost").toString();
    auto port = settings.value("serverPort", 9007).value<quint16>();
    settings.setValue("serverHost", hostName);
    settings.setValue("serverPort", port);
    settings.sync();

    model->connectToServer(hostName, port);

    auto loginWindow = new LoginWidget;
    connect(loginWindow, &LoginWidget::loginRequested, this,
            [this](const QString &username, const QString &password) { model->login(username, password); });
    addWidget(loginWindow);

    auto welcomeWindow = new WelcomeWidget;
    connect(welcomeWindow, &WelcomeWidget::begin, this, [this] { model->listFoodRequest(); });
    addWidget(welcomeWindow);

    auto orderwindow = new OrderWidget;
    connect(orderwindow, &OrderWidget::orderRequested, this,
            [this](const std::vector<FoodAmount> &ordered) { model->orderSend(ordered); });
    addWidget(orderwindow);

    auto paywindow = new PayWidget;
    connect(paywindow, &PayWidget::payed, this, [this] { model->paySend(); });
    addWidget(paywindow);

    auto endwindow = new EndWidget;
    addWidget(endwindow);

    connect(model, &Model::loginSucceded, this, [this, welcomeWindow] { setCurrentWidget(welcomeWindow); });
    connect(model, &Model::foodListRefreshed, this,
            [orderwindow](const std::vector<FoodContains> &foodList) { orderwindow->setFoodList(foodList); });
    connect(model, &Model::readyToOrder, this, [this, orderwindow] {
        setCurrentWidget(orderwindow);
        orderwindow->resetCart();
    });
    connect(model, &Model::orderSucceded, this, [this, paywindow](const std::vector<FoodContains> &foods) {
        paywindow->setFoodList(foods);
        setCurrentWidget(paywindow);
    });

    connect(model, &Model::statusChanged, this, [paywindow](OrderStatus status) { paywindow->setOrderStatus(status); });

    connect(model, &Model::paySucceded, this, [this, endwindow] { setCurrentWidget(endwindow); });
    connect(endwindow, &EndWidget::startAgain, this, [this, welcomeWindow] { setCurrentWidget(welcomeWindow); });

    auto networkErrorWidget = new QLabel(tr("Network Error"));
    addWidget(networkErrorWidget);
    setCurrentWidget(networkErrorWidget);

    connect(model, &Model::connectionStateChanged, this, [this, networkErrorWidget, loginWindow](bool isOnline) {
        if (!isOnline) {
            setCurrentWidget(networkErrorWidget);
        } else {
            setCurrentWidget(loginWindow);
        }
    });
    connect(model, &Model::loginFailed, this, [this] {
        QMessageBox::critical(this, tr("Login error"), tr("The supplied username or password was incorrect"));
    });

    connect(model, &Model::orderFailed, this, [this] {
        QMessageBox::critical(this, tr("Order error"), tr("The requested order is sold out, please choose another food"));
    });
    connect(model, &Model::payFailed, this, [this, paywindow] {
        QMessageBox::critical(this, tr("Pay error"), tr("The pay request is faiiled, please try again or ask help"));
        paywindow->payFailed();
    });
}

void MainWidget::on_connected() {
}

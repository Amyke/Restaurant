#include "LoginWidget.hpp"

#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

LoginWidget::LoginWidget(QWidget *parent) : QWidget(parent) {
    auto layout = new QVBoxLayout;

    auto userNameEdit = new QLineEdit;
    userNameEdit->setPlaceholderText(tr("User name"));

    auto passwordEdit = new QLineEdit;
    passwordEdit->setPlaceholderText(tr("Password"));

    loginButton_ = new QPushButton(tr("Login"));

    layout->addStretch();
    layout->addWidget(userNameEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(loginButton_);
    layout->addStretch();

    setLayout(layout);

    connect(loginButton_, &QPushButton::clicked, this,
            [this, userNameEdit, passwordEdit] { loginRequested(userNameEdit->text(), passwordEdit->text()); });
}
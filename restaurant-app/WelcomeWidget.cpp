#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "WelcomeWidget.hpp"

WelcomeWidget::WelcomeWidget(QWidget* parent) : QWidget(parent) {
    auto layout = new QVBoxLayout;

    auto welcome = new QLabel;
    welcome->setText(tr("Welcome to our Restaurant"));
    QFont font = welcome->font();
    font.setPointSize(52);
    font.setBold(true);
    welcome->setWordWrap(true);
    welcome->setFont(font);

    auto newOrderButton = new QPushButton(tr("New Order"));

    layout->addWidget(welcome);
    layout->addWidget(newOrderButton);

    setLayout(layout);

    connect(newOrderButton, &QPushButton::clicked, this, [this] { begin(); });
}
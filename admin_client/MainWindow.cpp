#include "MainWindow.hpp"

#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QLabel>

#include <QSettings>

#include "LoginWidget.hpp"
#include "Model.hpp"
#include "WorkspaceWidget.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), model(new Model(new Client(this), this)) {

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Restaurant", "admin");
    auto hostName = settings.value("serverHost", "localhost").toString();
    auto port = settings.value("serverPort", 9007).value<quint16>();
    settings.setValue("serverHost", hostName);
    settings.setValue("serverPort", port);
    settings.sync();

    model->connectToServer(hostName, port);

    auto loginWindow = new LoginWidget;
    connect(loginWindow, &LoginWidget::loginRequested, this,
            [this](const QString &username, const QString &password) { model->login(username, password); });

    auto workspace = new WorkspaceWidget(*model);

    auto widget = new QStackedWidget;
    widget->addWidget(loginWindow);
    widget->addWidget(workspace);
    setCentralWidget(widget);

    auto toolbar = addToolBar(tr("Tools"));
    toolbar->addAction(tr("Administration"), workspace, &WorkspaceWidget::requestAdministration);
    toolbar->addAction(tr("Diagramm"), workspace, &WorkspaceWidget::requestDiagramm);
    toolbar->addAction(tr("Order Details"), workspace, &WorkspaceWidget::requestOrderDetails);
    connect(widget, &QStackedWidget::currentChanged, this, [toolbar](int idx) {
        if (idx == 0) {
            toolbar->hide();
        } else {
            toolbar->show();
        }
    });
    toolbar->hide();

#ifndef NDEBUG
    auto debugMenu = menuBar()->addMenu("Debug");
    connect(debugMenu->addAction("Login"), &QAction::triggered, widget, [widget] { widget->setCurrentIndex(0); });
    connect(debugMenu->addAction("Workspace"), &QAction::triggered, widget, [widget] { widget->setCurrentIndex(1); });
#endif

    auto networkErrorWidget = new QLabel(tr("Network Error"));
    widget->addWidget(networkErrorWidget);
    widget->setCurrentWidget(networkErrorWidget);

    connect(model, &Model::connectionStateChanged, this, [widget, networkErrorWidget, loginWindow](bool isOnline) {
        if (!isOnline) {
            widget->setCurrentWidget(networkErrorWidget);
        } else {
            widget->setCurrentWidget(loginWindow);
        }
    });


    connect(model, &Model::loginSucceded, this, [widget, workspace] { widget->setCurrentWidget(workspace); });

    connect(model, &Model::loginFailed, this, [this] {
        QMessageBox::critical(this, tr("Login error"), tr("The login request is failed, please try again or ask help"));
    });
}

void MainWindow::on_connected() {
}

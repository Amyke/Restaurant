#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>

#include "MainWidget.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QMainWindow window;

    auto widget = new MainWidget;
    window.setCentralWidget(widget);

    auto debugMenu = window.menuBar()->addMenu("Debug");
    QObject::connect(debugMenu->addAction("Login"), &QAction::triggered, widget,
                     [widget] { widget->setCurrentIndex(0); });
    QObject::connect(debugMenu->addAction("Welcome"), &QAction::triggered, widget,
                     [widget] { widget->setCurrentIndex(1); });
    QObject::connect(debugMenu->addAction("Order"), &QAction::triggered, widget,
                     [widget] { widget->setCurrentIndex(2); });
    QObject::connect(debugMenu->addAction("Pay"), &QAction::triggered, widget,
                     [widget] { widget->setCurrentIndex(3); });
    QObject::connect(debugMenu->addAction("End"), &QAction::triggered, widget,
                     [widget] { widget->setCurrentIndex(4); });

    window.show();

    return app.exec();
}
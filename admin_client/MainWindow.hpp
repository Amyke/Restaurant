#ifndef MAINWIDGETADMIN_HPP
#define MAINWIDGETADMIN_HPP

#include <QtWidgets/QMainWindow>

class Model;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void on_connected();

private:
    Model *model;
};

#endif //! MAINWIDGETADMIN_HPP
#ifndef LOGINCLIENT_HPP
#define LOGINCLIENT_HPP

#include <QtWidgets/QStackedWidget>

class Model;
class QPushButton;

class LoginWidget : public QWidget {
    Q_OBJECT
public:
    explicit LoginWidget(QWidget *parent = nullptr);

signals:
    void loginRequested(const QString &username, const QString &password);

private:
    QPushButton *loginButton_;
};

#endif //! LOGINCLIENT_HPP
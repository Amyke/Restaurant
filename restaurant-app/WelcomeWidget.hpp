#ifndef WELCOMEWIDGET_HPP
#define WELCOMEWIDGET_HPP

#include <QtWidgets/QStackedWidget>

class WelcomeWidget : public QWidget {
    Q_OBJECT
public:
    explicit WelcomeWidget(QWidget *parent = nullptr);

signals:
    void begin();
};

#endif //! WELCOMEWIDGET_HPP
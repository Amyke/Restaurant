#ifndef MAINWIDGETCLIENT_HPP
#define MAINWIDGETCLIENT_HPP

#include <QtWidgets/QStackedWidget>

class Model;

class MainWidget : public QStackedWidget {
	Q_OBJECT
public:
	explicit MainWidget(QWidget* parent = nullptr);

private:
	void on_connected();

private:
	Model* model;
};

#endif //! MAINWIDGETCLIENT_HPP
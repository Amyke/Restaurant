#ifndef PAYWIDGET_HPP
#define PAYWIDGET_HPP

#include <QtWidgets/QStackedWidget>


class PayWidget : public QWidget {
	Q_OBJECT;
public:
	explicit PayWidget(QWidget* parent = nullptr);

signals:
	void payed();

private:
	void pay_request(/*hey mate, i want to pay*/);

};

#endif //! PAYWIDGET_HPP
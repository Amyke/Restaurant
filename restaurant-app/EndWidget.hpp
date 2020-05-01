#ifndef ENDWIDGET_HPP
#define ENDWIDGET_HPP

#include <QtWidgets/QWidget>


class EndWidget : public QWidget {
	Q_OBJECT
public:
	explicit EndWidget(QWidget* parent = nullptr);
signals:
	void startAgain();
};


#endif //! ENDWIDGET_HPP
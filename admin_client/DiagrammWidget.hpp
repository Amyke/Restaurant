#ifndef DIAGRAMMWIDGET_HPP
#define DIAGRAMMWIDGET_HPP

#include <QtWidgets/QStackedWidget>


class DiagrammWidget : public QWidget {
	Q_OBJECT
public:
	explicit DiagrammWidget(QWidget* parent = nullptr);
signals:
    void foodsDataRequest(const QDate &begin, const QDate &end);

private:

};

#endif //! DIAGRAMMWIDGETADMIN_HPP
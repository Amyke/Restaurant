#include "DiagrammWidget.hpp"

#include <QCalendarWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

#include <QtCore/QAbstractTableModel>

DiagrammWidget::DiagrammWidget(QWidget *parent) : QWidget(parent) {

    auto layout = new QVBoxLayout;
    auto label = new QLabel(tr("If you wanto to make a diagramm, pls select first of all a begin and end date"));

    auto calendarLayout = new QVBoxLayout;
    auto now = QDateTime::currentDateTime();

    auto *beginDate = new QCalendarWidget;
    beginDate->setGridVisible(true);
   // beginDate->setMinimumDate(QDate(2012, 12, 12));
   // beginDate->setMaximumDate(QDate(now.date().year, now.date().month, now.date().day));

    auto *endDate = new QCalendarWidget;
    endDate->setGridVisible(true);
  //  beginDate->setMinimumDate(QDate(2012, 12, 12));
    //endDate->setMaximumDate(QDate(now.date().year, now.date().month, now.date().day));

    calendarLayout->addWidget(beginDate);
    calendarLayout->addWidget(endDate);

    auto buttonLayout = new QHBoxLayout;

    auto foodButton = new QPushButton(tr("Do it"));

    buttonLayout->addWidget(foodButton);

    layout->addWidget(label);
    layout->addLayout(calendarLayout, 1);
    layout->addLayout(buttonLayout, 1);

    setLayout(layout);

    connect(foodButton, &QPushButton::clicked, this,
            [this, beginDate, endDate] { foodsDataRequest(beginDate->selectedDate(), endDate->selectedDate()); });
}

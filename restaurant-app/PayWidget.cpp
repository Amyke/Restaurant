#include "PayWidget.hpp"
#include "OrderModel.hpp"

#include <QtWidgets/QLabel>
#include <QtWidgets/QTableView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets\QMessageBox>

#include <QtCore/QAbstractTableModel>

#include "Model.hpp"


PayWidget::PayWidget(QWidget* parent) : QWidget(parent) {
	
	auto layout = new QVBoxLayout;


	auto selectFood = new QTableView;
	auto model = new OrderModel(selectFood);
	selectFood->setModel(model);
	selectFood->setStyleSheet(" QTableViewEdit {border-style: outset}");


	auto payButton = new QPushButton(tr("Pay"));

	qint32 price = 0;
	/*for each (auto item in PayWidgetClientModel::order)
	{
		price += (item.FoodPrice * item.Amount);
	}*/
	auto allPrice = new QLabel("Price: " + price, this);


	layout->addWidget(selectFood);
	layout->addWidget(payButton);
	layout->addWidget(allPrice);

	setLayout(layout);


	connect(payButton, &QPushButton::clicked, this,
		[this, selectFood] {
			payed();	

			pay_request();
		});
}

void PayWidget::pay_request()
{
	QMessageBox msgBox;
	msgBox.setText("Your request has been sent! (= ");
	msgBox.exec();

	msgBox.Abort;
}

#include "EndWidget.hpp"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>


EndWidget::EndWidget(QWidget* parent) : QWidget(parent) {
	auto layout = new QVBoxLayout;


	auto byebye = new QLabel;
	byebye->setText(tr("We hope you enjoy the time what spend here, bye"));
	QFont font = byebye->font();
	font.setPointSize(52);
	font.setBold(true);
	byebye->setWordWrap(true);
	byebye->setFont(font);

	auto weolcomePage = new QPushButton(tr("Welcome page"));
	const QSize button_size = QSize(140, 80);
	weolcomePage->setMaximumSize(button_size);
	weolcomePage->move(1000, 0);

	layout->addWidget(byebye);
	layout->addWidget(weolcomePage);

	setLayout(layout);


	connect(weolcomePage, &QPushButton::clicked, this,
		[this] {

			startAgain();
		});

}
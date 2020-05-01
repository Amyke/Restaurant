#include "AdministrationWidget.hpp"
#include "FoodModel.hpp"

#include <QMessageBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>

#include <QtCore/QAbstractTableModel>

AdministrationWidget::AdministrationWidget(QWidget *parent) : QWidget(parent) {
    auto layout = new QVBoxLayout;

    foodModel_ = new FoodModel(this);

    auto selectFood = new QTableView;

    selectFood->setSelectionBehavior(QTableView::SelectItems);
    selectFood->setSelectionMode(QTableView::SingleSelection);

    selectFood->setModel(foodModel_);

    auto addButton = new QPushButton(tr("Add new food"));
    auto revertButton = new QPushButton(tr("Revert changes"));
    auto saveButton = new QPushButton(tr("Save changes"));

    layout->addWidget(selectFood);
    layout->addWidget(addButton);
    layout->addWidget(revertButton);
    layout->addWidget(saveButton);

    setLayout(layout);

    connect(addButton, &QPushButton::clicked, this, [this] { foodModel_->insertRow(foodModel_->rowCount()); });

    connect(revertButton, &QPushButton::clicked, this, [this] {
        QMessageBox::StandardButton button = QMessageBox::question(
            this, tr("Revert changes"), tr("Are you sure that you want to revert the changes?"));
        if (button == QMessageBox::StandardButton::Yes) {
            foodModel_->resetData();
        }
    });

    connect(saveButton, &QPushButton::clicked, this, [this] {
        QMessageBox::StandardButton button = QMessageBox::question(
            this, tr("Food Changes Required!"), tr("Are you sure that you want to save the modified foods?"));
        if (button == QMessageBox::StandardButton::Yes) {
            // todo

            auto changes = deltaOfTheFoods();
            changedFood(changes);

        }
    });
}

void AdministrationWidget::setFoodList(const std::vector<Food> &foodList) {
    foodModel_->setFoodList(foodList);
}

const Delta AdministrationWidget::deltaOfTheFoods() {
    return foodModel_->computeDelta();
}

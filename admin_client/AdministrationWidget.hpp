#ifndef ADMINISTRATIONWIDGET_HPP
#define ADMINISTRATIONWIDGET_HPP

#include <vector>

#include "Message.hpp"
#include <QtWidgets/QStackedWidget>

class FoodModel;

Q_DECLARE_METATYPE(Delta);

class AdministrationWidget : public QWidget {
    Q_OBJECT
public:
    explicit AdministrationWidget(QWidget *parent = nullptr);

    void setFoodList(const std::vector<Food> &foodList);

    const Delta deltaOfTheFoods();
signals:
    void changedFood(const Delta &delta);

private:
private:
    FoodModel *foodModel_;
};

#endif //! ADMINISTRATIONWIDGET_HPP
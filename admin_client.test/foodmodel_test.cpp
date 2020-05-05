#include "FoodModel.hpp"

#include <QtTest/QtTest>

Q_DECLARE_METATYPE(Food);
Q_DECLARE_METATYPE(std::vector<Food>);

class FoodModelTest : public QObject {
    Q_OBJECT
private slots:
    void initTestCase() {
        qRegisterMetaType<Food>();
        qRegisterMetaType<std::vector<Food>>();
    }

    void init() {
        foods_ = new FoodModel;
    }
    void cleanup() {
        delete foods_;
    }

    void InsertNewRow() {
        auto original = foods_->rowCount();
        foods_->insertRows(foods_->rowCount(), 1);

        QCOMPARE(original + 1, foods_->rowCount());
    }
    void SetDataSimulate() {
        std::vector<Food> foodList;
        foodList.push_back({{1, "Kaja", 1, 2}, true});
        foods_->setFoodList(foodList);

        QCOMPARE(1, foods_->rowCount());
    }

    void DeltaSimulateInsert() {

        std::vector<Food> foodList;
        foodList.push_back({{1, "Kaja", 1, 2}, true});
        foods_->setFoodList(foodList);

        foods_->insertRow(foods_->rowCount());

        auto newFood = foods_->computeDelta();

        Food insretrowData = {{0, "EditMe", 0, 0}, false};
        auto inserted = newFood.CreatedFoods[0];

        QCOMPARE(inserted.FoodData.FoodId, insretrowData.FoodData.FoodId);
    }
    void DeltaSimulateModified() {
        std::vector<Food> foodList;
        foodList.push_back({{1, "Kaja", 1, 2}, true});
        foods_->setFoodList(foodList);

        foods_->setData(foods_->index(0, 0), QString("Palacsinta"));

        auto newFood = foods_->computeDelta();

        auto modified = newFood.ModifiedFoods[0];

        QCOMPARE(modified.FoodData.FoodName, "Palacsinta");
    }

    void DeltaInsertAndModified() {
        std::vector<Food> foodList;
        foodList.push_back({{1, "Kaja", 1, 2}, true});
        foods_->setFoodList(foodList);

        foods_->insertRow(foods_->rowCount());
        foods_->insertRow(foods_->rowCount());
        foods_->setData(foods_->index(0, 0), QString("Palacsinta"));

        Food insretrowData = {{0, "EditMe", 0, 0}, false};

        // Act
        auto newFood = foods_->computeDelta();
        auto modified = newFood.ModifiedFoods[0];
        auto inserted1 = newFood.CreatedFoods[0];
        auto inserted2 = newFood.CreatedFoods[1];

        QCOMPARE(modified.FoodData.FoodName, "Palacsinta");
        QCOMPARE(inserted1.FoodData.FoodId, insretrowData.FoodData.FoodId);
        QCOMPARE(inserted2.Visible, insretrowData.Visible);
    }

    void ResetCahnges() {

        std::vector<Food> foodList;
        foodList.push_back({{1, "Kaja", 1, 2}, true});
        foods_->setFoodList(foodList);

        foods_->insertRow(foods_->rowCount());
        foods_->insertRow(foods_->rowCount());
        foods_->setData(foods_->index(0, 0), QString("Palacsinta"));

        QCOMPARE(foods_->rowCount(), 3);

        foods_->resetData();

        QCOMPARE(foods_->rowCount(), 1);
    }

private:
    FoodModel *foods_;
};
QTEST_MAIN(FoodModelTest);
#include "foodmodel_test.moc"
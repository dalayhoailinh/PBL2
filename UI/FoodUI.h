#ifndef FOOD_UI_H
#define FOOD_UI_H
#include "../BLL/FoodBLL.h"
#include "../BLL/CategoryBLL.h"
#include "../BLL/Result.h"
using namespace std;

class FoodUI {
private:
    FoodBLL& foodBll;
    CategoryBLL& categoryBll;

    void printFoodHeader();
    void printFoodRow(const Food& f);

public:
	FoodUI(FoodBLL& fBll, CategoryBLL& cBll) : foodBll(fBll), categoryBll(cBll) {};
    void menu();
    void listFoods();
    void addFood();
    void editFood();
    void deleteFood();
};
#endif


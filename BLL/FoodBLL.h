#ifndef FOOD_BLL_H
#define FOOD_BLL_H
#include "../DAL/FoodDAL.h"
#include "../DAL/CategoryDAL.h"
#include "../Models/Food.h"
#include "Result.h"
#include <vector>
using namespace std;

class FoodBLL {
private:
    FoodDAL& foodDal;
    CategoryDAL& categoryDal;

    Result validateName(const string& name);
    Result validateCategory(int categoryId);
    Result validatePrice(double price);

public:
    FoodBLL(FoodDAL& fDal, CategoryDAL& cDal) : foodDal(fDal), categoryDal(cDal) {};
    vector<Food> getAll();
    bool getById(int id, Food& out);

    Result addChecked(const Food& food);
    Result updateChecked(const Food& food);
    Result removeChecked(int id);
};
#endif


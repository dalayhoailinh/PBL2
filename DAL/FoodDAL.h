#ifndef FOOD_DAL_H
#define FOOD_DAL_H
#include "DatabaseConnection.h"
#include "../Models/Food.h"
#include <vector>
using namespace std;

class FoodDAL {
	public:
	    vector<Food> getAll();
	    bool getById(int id, Food& out);
	    bool insert(const Food& food);
	    bool update(const Food& food);
	    bool remove(int id);
};
#endif


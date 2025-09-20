#ifndef CATEGORY_DAL_H
#define CATEGORY_DAL_H
#include "DatabaseConnection.h"
#include "../Models/Category.h"
#include <vector>
using namespace std;

class CategoryDAL {
	public:
	    vector<Category> getAll();
	    bool getById(int id, Category& out);
};
#endif


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
		bool insert(const Category& category);
		bool update(const Category& category);
		bool remove(int id);
};
#endif


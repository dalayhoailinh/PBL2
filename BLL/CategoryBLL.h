#ifndef CATEGORY_BLL_H
#define CATEGORY_BLL_H
#include "../DAL/CategoryDAL.h"
#include "../Models/Category.h"
#include "Result.h"
#include <vector>
using namespace std;

class CategoryBLL {
	private:
		CategoryDAL& categoryDal;
		
		Result validateName(const string& name);
	public:
		CategoryBLL(CategoryDAL& cDal) : categoryDal(cDal) {};
		vector<Category> getAll();
		bool getById(int id, Category& out);
		
		Result addChecked(const Category& category);
		Result updateChecked(const Category& category);
		Result removeChecked(int id);
};
#endif


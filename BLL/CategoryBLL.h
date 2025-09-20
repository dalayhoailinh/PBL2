#ifndef CATEGORY_BLL_H
#define CATEGORY_BLL_H
#include "../DAL/CategoryDAL.h"
#include "../Models/Category.h"
#include <vector>
using namespace std;

class CategoryBLL {
private:
    CategoryDAL& categoryDal;
public:
	CategoryBLL(CategoryDAL& cDal) : categoryDal(cDal) {};
    vector<Category> getAll();
    bool getById(int id, Category& out);
};
#endif


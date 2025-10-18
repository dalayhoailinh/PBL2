#ifndef CATEGORY_UI_H
#define CATEGORY_UI_H
#include "../BLL/CategoryBLL.h"
#include "../BLL/Result.h"
using namespace std;

class CategoryUI {
	private:
		CategoryBLL& categoryBll;

		void printCategoryHeader();
		void printCategoryRow(const Category& c);

	public:
		CategoryUI(CategoryBLL& cBll) : categoryBll(cBll) {};
		void menu();
		void listCategories();
		void addCategory();
		void editCategory();
		void deleteCategory();
};
#endif

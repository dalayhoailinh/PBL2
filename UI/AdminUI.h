#ifndef ADMIN_UI_H
#define ADMIN_UI_H
#include <iostream>
#include "StaffUI.h"
#include "../BLL/UserBLL.h"
#include "../UI/FoodUI.h"
#include "../UI/CategoryUI.h"
#include "../UI/TableUI.h"
using namespace std;

class AdminUI {
	private:
	    StaffUI& staffUi;
	    UserBLL& userBll;
	    FoodUI& foodUi;
	    CategoryUI& categoryUi;
	    TableUI& tableUi;
	    UserUI& userUi;
	    
	public:
	    AdminUI (
			StaffUI& sUi, 
			UserBLL& uBll, 
			FoodUI& fUi,
			CategoryUI& cUi, 
			TableUI& tUi, 
			UserUI& uUi) 
		: 	staffUi(sUi), 
			userBll(uBll), 
			foodUi(fUi), 
			categoryUi(cUi),
			tableUi(tUi), 
			userUi(uUi) {}
	
	    void menu();
};
#endif


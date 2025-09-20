#ifndef ADMIN_UI_H
#define ADMIN_UI_H
#include <iostream>
#include "StaffUI.h"
#include "../BLL/UserBLL.h"
#include "../UI/FoodUI.h"
using namespace std;

class AdminUI {
	private:
	    StaffUI& staffUi;
	    UserBLL& userBll;
	    FoodUI& foodUi;
	    UserUI& userUi;
	    
	public:
	    AdminUI(StaffUI& sUi, UserBLL& uBll, FoodUI& fUi, UserUI& uUi) : staffUi(sUi), userBll(uBll), foodUi(fUi), userUi(uUi) {}
	
	    void menu();
};
#endif


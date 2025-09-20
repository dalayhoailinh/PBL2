#ifndef STAFF_UI_H
#define STAFF_UI_H
#include <iostream>
#include "../BLL/UserBLL.h"
#include "../UI/UserUI.h"
using namespace std;

class StaffUI {
private:
    UserBLL& userBll;
    UserUI& userUi;
public:
    StaffUI(UserBLL& uBll, UserUI& uUi) : userBll(uBll), userUi(uUi) {}
    void menu(bool isAdminView = false);
};
#endif


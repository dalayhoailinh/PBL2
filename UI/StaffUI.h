#ifndef STAFF_UI_H
#define STAFF_UI_H

#include "../BLL/BillBLL.h"
#include "../BLL/UserBLL.h"
#include "../Models/Bill.h"
#include "../Models/BillItem.h"
#include "../UI/FoodUI.h"
#include "../UI/TableUI.h"
#include "../UI/UserUI.h"
#include <vector>
using namespace std;

class StaffUI {
private:
    UserBLL &userBll;
    UserUI &userUi;
    FoodUI &foodUi;
    TableUI &tableUi;
    BillBLL &billBll;

    void chooseTableFlow(bool isAdminView);
    void tableOrderMenu(int tableId);
    void showCurrentOrder(int tableId);
    void addFoodFlow(int tableId);
    void updateItemFlow(int tableId);
    void deleteItemFlow(int tableId);
    void checkoutFlow(int tableId);

public:
    StaffUI(UserBLL &uBll, BillBLL &bBll, UserUI &uUi, FoodUI &fUi, TableUI &tUi)
        : userBll(uBll), billBll(bBll), userUi(uUi), foodUi(fUi), tableUi(tUi) {}

    void menu(bool isAdminView = false);
};
#endif


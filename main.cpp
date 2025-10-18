#include "BLL/CategoryBLL.h"
#include "BLL/FoodBLL.h"
#include "BLL/TableBLL.h"
#include "BLL/UserBLL.h"
#include "BLL/BillBLL.h"
#include "DAL/CategoryDAL.h"
#include "DAL/FoodDAL.h"
#include "DAL/TableDAL.h"
#include "DAL/UserDAL.h"
#include "DAL/BillDAL.h"
#include "UI/AdminUI.h"
#include "UI/FoodUI.h"
#include "UI/CategoryUI.h"
#include "UI/StaffUI.h"
#include "UI/TableUI.h"
#include "UI/UserUI.h"

int main() {
    FoodDAL foodDal;
    CategoryDAL categoryDal;
    UserDAL userDal;
    TableDAL tableDal;
    BillDAL billDal;

    FoodBLL foodBll(foodDal, categoryDal);
    CategoryBLL categoryBll(categoryDal);
    UserBLL userBll(userDal);
    TableBLL tableBll(tableDal);
    BillBLL billBll(billDal, tableDal);

    FoodUI foodUi(foodBll, categoryBll);
    CategoryUI categoryUi(categoryBll);
    UserUI userUi(userBll);
    TableUI tableUi(tableBll);
    StaffUI staffUi(userBll, billBll, userUi, foodUi, tableUi);
    AdminUI adminUi(staffUi, userBll, foodUi, categoryUi, tableUi, userUi);

    while (true) {
        userUi.loginMenu();
        User u = userBll.getCurrentUser();
        if (u.id == 0)
            continue;
        if (u.id == -1)
            break;

        if (u.roleId == 1) {
            adminUi.menu();
        } else {
            staffUi.menu(false);
        }
    }
}

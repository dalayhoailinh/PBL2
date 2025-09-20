#include "DAL/FoodDAL.h"
#include "DAL/CategoryDAL.h"
#include "DAL/UserDAL.h"
#include "BLL/FoodBLL.h"
#include "BLL/CategoryBLL.h"
#include "BLL/UserBLL.h"
#include "UI/FoodUI.h"
#include "UI/UserUI.h"
#include "UI/AdminUI.h"
#include "UI/StaffUI.h"

int main() {
    FoodDAL foodDal;
    CategoryDAL categoryDal;
    UserDAL userDal;

    FoodBLL foodBll(foodDal, categoryDal);
    CategoryBLL categoryBll(categoryDal);
    UserBLL userBll(userDal);

    FoodUI foodUi(foodBll, categoryBll);
    UserUI userUi(userBll);
    StaffUI staffUi(userBll, userUi);
    AdminUI adminUi(staffUi, userBll, foodUi, userUi);

    while (true) {
        userUi.loginMenu();
        User u = userBll.getCurrentUser();
        if (u.id == 0) continue;

        if (u.roleId == 1) {
            adminUi.menu();
        } else {
            staffUi.menu(false);
        }
    }
}


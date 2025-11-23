#include "BLL/BillBLL.h"
#include "BLL/CategoryBLL.h"
#include "BLL/FoodBLL.h"
#include "BLL/TableBLL.h"
#include "BLL/UserBLL.h"
#include "DAL/BillDAL.h"
#include "DAL/CategoryDAL.h"
#include "DAL/FoodDAL.h"
#include "DAL/TableDAL.h"
#include "DAL/UserDAL.h"
#include "UI/MainUI.h"
#include <cstdio>
#include <iostream>

using namespace std;
int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);
    setvbuf(stdout, NULL, _IOFBF, 1 << 20);
    FoodDAL foodDal;
    CategoryDAL categoryDal;
    TableDAL tableDal;
    BillDAL billDal;

    FoodBLL foodBll(foodDal, categoryDal);
    CategoryBLL categoryBll(categoryDal);
    TableBLL tableBll(tableDal);
    BillBLL billBll(billDal, tableDal);

    MainUI app(foodBll, categoryBll);
    app.run();
}

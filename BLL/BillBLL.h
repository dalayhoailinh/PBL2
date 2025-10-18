#ifndef BILL_BLL_H
#define BILL_BLL_H

#include "../DAL/BillDAL.h"
#include "../DAL/TableDAL.h"
#include <string>
#include <vector>
using namespace std;

class BillBLL {
    public:
        BillBLL(BillDAL &bDal, TableDAL &tDal) : billDal(bDal), tableDal(tDal) {}

        bool getOpenBillByTableId(int tableId, Bill &out);
        // C++98: dùng NULL thay cho nullptr
        bool getCurrentItemsOfTable(int tableId, vector<BillItem> &outItems,
                                    Bill *outBill = NULL, double *outTotal = NULL);
        bool checkoutTable(int tableId);
        bool addFoodToTable(int tableId, int foodId, int quantity, const string &desc);
        bool updateBillItem(int billItemId, int quantity, const string &desc);
        bool deleteBillItem(int billItemId);

    private:
        BillDAL &billDal;
        TableDAL &tableDal;
};
#endif

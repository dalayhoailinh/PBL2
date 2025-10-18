#ifndef BILL_DAL_H
#define BILL_DAL_H
#include "../Models/Bill.h"
#include "../Models/BillItem.h"
#include "DatabaseConnection.h"
#include <string>
#include <vector>
using namespace std;

class BillDAL {
    public:
        bool getOpenByTableId(int tableId, Bill &out);
        int createForTable(int tableId);
        bool deleteBill(int billId);
        bool recalcTotal(int billId);
        bool addItem(int billId, int foodId, int quantity, const string &desc, int &outNewId);
        bool updateItem(int billItemId, int quantity, const string &desc);
        bool removeItem(int billItemId);
        bool getItemContext(int billItemId, int &billId, int &tableId);
        bool listItemsByBill(int billId, vector<BillItem> &out);
        bool closeBill(int billId);
        int countItems(int billId);

    private:
        bool getFoodPrice(int foodId, double &price);
};
#endif

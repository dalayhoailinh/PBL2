#include "BillBLL.h"
#include <stdexcept>

bool BillBLL::getOpenBillByTableId(int tableId, Bill &out) {
    return billDal.getOpenByTableId(tableId, out);
}

bool BillBLL::getCurrentItemsOfTable(int tableId, vector<BillItem> &outItems, Bill *outBill, double *outTotal) {
    Bill b;
    if (!billDal.getOpenByTableId(tableId, b))
        return false;
    outItems.clear();
    billDal.listItemsByBill(b.id, outItems);
    billDal.recalcTotal(b.id);
    if (outBill)
        *outBill = b;
    if (outTotal) {
        Bill tmp;
        billDal.getOpenByTableId(tableId, tmp);
        *outTotal = tmp.total_price;
    }
    return true;
}

bool BillBLL::checkoutTable(int tableId) {
    Bill b;
    if (!billDal.getOpenByTableId(tableId, b))
        return false;
    billDal.recalcTotal(b.id);
    billDal.closeBill(b.id);
    Table t;
    if (tableDal.getById(tableId, t)) {
        t.status_id = 0;
        tableDal.update(t);
    }
    return true;
}

bool BillBLL::addFoodToTable(int tableId, int foodId, int quantity, const string &desc) {

    Bill bill;
    int billId = -1;
    if (billDal.getOpenByTableId(tableId, bill)) {
        billId = bill.id;
    } else {
        billId = billDal.createForTable(tableId);
        if (billId <= 0)
            throw runtime_error("create bill failed");
    }

    int newItemId = -1;
    if (!billDal.addItem(billId, foodId, quantity, desc, newItemId))
        throw runtime_error("insert bill_item failed");

    billDal.recalcTotal(billId);

    Table t;
    if (tableDal.getById(tableId, t)) {
        t.status_id = 1;
        tableDal.update(t);
    }
    return true;
}

bool BillBLL::updateBillItem(int billItemId, int quantity, const string &desc) {

    if (!billDal.updateItem(billItemId, quantity, desc))
        return false;

    int billId = 0, tableId = 0;
    if (billDal.getItemContext(billItemId, billId, tableId)) {
        billDal.recalcTotal(billId);
    }
    return true;
}

bool BillBLL::deleteBillItem(int billItemId) {
    int billId = 0, tableId = 0;
    if (!billDal.getItemContext(billItemId, billId, tableId))
        return false;

    if (!billDal.removeItem(billItemId))
        return false;

    int cnt = billDal.countItems(billId);
    if (cnt <= 0) {
        billDal.deleteBill(billId);
        Table t;
        if (tableDal.getById(tableId, t)) {
            t.status_id = 0;
            tableDal.update(t);
        }
    } else {
        billDal.recalcTotal(billId);
    }
    return true;
}

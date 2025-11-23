#include "BillBLL.h"
using namespace std;

Result BillBLL::validateTable(int tableId) {
    Result r; r.ok=false; r.message="Ban khong hop le";
    if (tableId <= 0) { r.message="TableId phai > 0"; return r; }
    Table t;
    if (!tableDal.getById(tableId, t)) { r.message="Ban khong ton tai"; return r; }
    r.ok=true; r.message=""; return r;
}

Result BillBLL::validateFoodId(int foodId) {
    Result r; r.ok=false; r.message="Mon khong hop le";
    if (foodId <= 0) { r.message="FoodId phai > 0"; return r; }
    r.ok=true; r.message=""; return r;
}

Result BillBLL::validateQuantity(int quantity) {
    Result r; r.ok=false; r.message="So luong khong hop le";
    if (quantity <= 0) { r.message="So luong phai > 0"; return r; }
    r.ok=true; r.message=""; return r;
}

Result BillBLL::validateDesc(const string& desc) {
    Result r; r.ok=false; r.message="Mo ta khong hop le";
    if (desc.size() > 200) { r.message="Mo ta qua dai (<=200 ky tu)"; return r; }
    r.ok=true; r.message=""; return r;
}

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

    if (outBill) *outBill = b;
    if (outTotal) {
        Bill tmp;
        billDal.getOpenByTableId(tableId, tmp);
        *outTotal = tmp.total_price;
    }
    return true;
}

Result BillBLL::checkoutTableChecked(int tableId) {
    Result r;

    r = validateTable(tableId); if (!r.ok) return r;

    Bill b;
    if (!billDal.getOpenByTableId(tableId, b)) {
        r.ok=false; r.message="Khong co bill mo cho ban nay";
        return r;
    }

    billDal.recalcTotal(b.id);
    billDal.closeBill(b.id);

    Table t;
    if (tableDal.getById(tableId, t)) {
        t.status_id = 0;
        tableDal.update(t);
    }

    r.ok=true; r.message="Checkout thanh cong";
    return r;
}

Result BillBLL::addFoodToTableChecked(int tableId, int foodId, int quantity, const string &desc) {
    Result r;

    r = validateTable(tableId); if (!r.ok) return r;
    r = validateFoodId(foodId); if (!r.ok) return r;
    r = validateQuantity(quantity); if (!r.ok) return r;
    r = validateDesc(desc); if (!r.ok) return r;

    Bill bill;
    int billId = -1;

    if (billDal.getOpenByTableId(tableId, bill)) {
        billId = bill.id;
    } else {
        billId = billDal.createForTable(tableId);
        if (billId <= 0) {
            r.ok=false; r.message="Tao bill that bai";
            return r;
        }
    }

    int newItemId = -1;
    if (!billDal.addItem(billId, foodId, quantity, desc, newItemId)) {
        r.ok=false; r.message="Them mon vao bill that bai";
        return r;
    }

    billDal.recalcTotal(billId);

    Table t;
    if (tableDal.getById(tableId, t)) {
        t.status_id = 1;
        tableDal.update(t);
    }

    r.ok=true; r.message="Them mon thanh cong";
    return r;
}

Result BillBLL::updateBillItemChecked(int billItemId, int quantity, const string &desc) {
    Result r;

    r = validateQuantity(quantity); if (!r.ok) return r;
    r = validateDesc(desc); if (!r.ok) return r;

    if (!billDal.updateItem(billItemId, quantity, desc)) {
        r.ok=false; r.message="Cap nhat item that bai";
        return r;
    }

    int billId = 0, tableId = 0;
    if (billDal.getItemContext(billItemId, billId, tableId)) {
        billDal.recalcTotal(billId);
    }

    r.ok=true; r.message="Cap nhat thanh cong";
    return r;
}

Result BillBLL::deleteBillItemChecked(int billItemId) {
    Result r;

    int billId = 0, tableId = 0;
    if (!billDal.getItemContext(billItemId, billId, tableId)) {
        r.ok=false; r.message="Khong tim thay item";
        return r;
    }

    if (!billDal.removeItem(billItemId)) {
        r.ok=false; r.message="Xoa item that bai";
        return r;
    }

    int cnt = billDal.countItems(billId);
    if (cnt <= 0) {
        billDal.deleteBill(billId);
        Table t;
        if (tableDal.getById(tableId, t)) {
            t.status_id = 0;
            tableDal.update(t);
        }
        r.ok=true; r.message="Xoa item thanh cong (Bill rong -> dong bill & giai phong ban)";
        return r;
    }

    billDal.recalcTotal(billId);
    r.ok=true; r.message="Xoa item thanh cong";
    return r;
}

bool BillBLL::checkoutTable(int tableId) {
    Result r = checkoutTableChecked(tableId);
    return r.ok;
}

bool BillBLL::addFoodToTable(int tableId, int foodId, int quantity, const string &desc) {
    Result r = addFoodToTableChecked(tableId, foodId, quantity, desc);
    return r.ok;
}

bool BillBLL::updateBillItem(int billItemId, int quantity, const string &desc) {
    Result r = updateBillItemChecked(billItemId, quantity, desc);
    return r.ok;
}

bool BillBLL::deleteBillItem(int billItemId) {
    Result r = deleteBillItemChecked(billItemId);
    return r.ok;
}


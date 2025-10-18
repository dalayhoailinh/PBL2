#include "StaffUI.h"
#include <iomanip>
#include <iostream>
using namespace std;

void StaffUI::menu(bool isAdminView) {
    while (true) {
        cout << "\n=== Staff Menu ===\n";
        cout << "1. Chon ban\n";
        cout << "2. Xem thong tin ca nhan\n";
        if (isAdminView)
            cout << "3. Chuyen sang giao dien Admin\n";
        if (isAdminView)
			cout << "0. Quay lai\n";
		else cout << "0. Dang xuat\n";
        cout << "Chon: ";
        int c;
        if (!(cin >> c))
            return;
        cin.ignore();

        if (c == 1)
            chooseTableFlow(isAdminView);
        else if (c == 2)
            userUi.profileMenu();
        else if (isAdminView && c == 3)
            return;
        else if (c == 0) {
            if (!isAdminView) userBll.logOut();
            break;
        } else
            cout << "Lua chon khong hop le\n";
    }
}

void StaffUI::chooseTableFlow(bool /*isAdminView*/) {
    cout << "\n=== CHON BAN ===\n";
    tableUi.listTables();
    cout << "Nhap table_id can thao tac: ";
    int tableId;
    if (!(cin >> tableId))
        return;
    cin.ignore();
    tableOrderMenu(tableId);
}

static void printItemsHeader() {
    cout << left
         << setw(10) << "ItemID"
         << setw(10) << "FoodID"
         << setw(10) << "Qty"
         << setw(12) << "SubTotal"
         << "Mo ta" << "\n";
    cout << string(60, '-') << "\n";
}

void StaffUI::showCurrentOrder(int tableId) {
    try {
        Bill bill;
        vector<BillItem> items;
        double total = 0.0;
        bool ok = billBll.getCurrentItemsOfTable(tableId, items, &bill, &total);
        if (!ok) {
            cout << "Ban chua co bill dang mo.\n";
            return;
        }
        cout << "\n=== ORDER HIEN TAI (Table #" << tableId << ", BillID " << bill.id << ") ===\n";
        if (items.empty()) {
            cout << "Chua co mon nao.\n";
            return;
        }
        printItemsHeader();

        // C++98: thay range-for bằng for chỉ số
        for (size_t i = 0; i < items.size(); ++i) {
            BillItem &it = items[i];
            cout << left
                 << setw(10) << it.id
                 << setw(10) << it.food_id
                 << setw(10) << it.quantity
                 << setw(12) << fixed << setprecision(2) << it.sub_total
                 << it.description << "\n";
        }
        cout << string(60, '=') << "\nTong cong: " << fixed << setprecision(2) << total << "\n";
    } catch (const exception &e) {
        cout << "Loi: " << e.what() << "\n";
    }
}

void StaffUI::addFoodFlow(int tableId) {
    try {
        cout << "\n=== THEM MON ===\n";
        foodUi.listFoods();
        int foodId, qty;
        string desc;
        cout << "Nhap food_id: ";
        cin >> foodId;
        cout << "Nhap so luong: ";
        cin >> qty;
        cin.ignore();
        cout << "Mo ta (co the de trong): ";
        getline(cin, desc);

        billBll.addFoodToTable(tableId, foodId, qty, desc);
        cout << ">> Da them mon vao ban " << tableId << ". Trang thai ban = 1 (dang su dung).\n";
        showCurrentOrder(tableId);
    } catch (const exception &e) {
        cout << "Loi khi them mon: " << e.what() << "\n";
    }
}

void StaffUI::updateItemFlow(int tableId) {
    try {
        cout << "\n=== CAP NHAT MON ===\n";
        showCurrentOrder(tableId);
        int itemId, qty;
        string desc;
        cout << "Nhap bill_item_id can sua: ";
        cin >> itemId;
        cout << "So luong moi: ";
        cin >> qty;
        cin.ignore();
        cout << "Mo ta moi (co the de trong): ";
        getline(cin, desc);

        if (billBll.updateBillItem(itemId, qty, desc))
            cout << ">> Da cap nhat.\n";
        else
            cout << ">> Khong cap nhat duoc (kiem tra ID?).\n";
        showCurrentOrder(tableId);
    } catch (const exception &e) {
        cout << "Loi cap nhat mon: " << e.what() << "\n";
    }
}

void StaffUI::deleteItemFlow(int tableId) {
    try {
        cout << "\n=== XOA MON ===\n";
        showCurrentOrder(tableId);
        cout << "Nhap bill_item_id can xoa: ";
        int itemId;
        if (!(cin >> itemId))
            return;
        cin.ignore();

        if (billBll.deleteBillItem(itemId)) {
            cout << ">> Da xoa mon.\n";
        } else {
            cout << ">> Khong xoa duoc (kiem tra ID?).\n";
        }
        showCurrentOrder(tableId);
    } catch (const exception &e) {
        cout << "Loi xoa mon: " << e.what() << "\n";
    }
}

void StaffUI::checkoutFlow(int tableId) {
    try {
        cout << "\n=== THANH TOAN ===\n";
        showCurrentOrder(tableId);
        cout << "Xac nhan thanh toan? (y/n): ";
        char cf;
        cin >> cf;
        cin.ignore();
        if (cf != 'y' && cf != 'Y') {
            cout << "Da huy thanh toan.\n";
            return;
        }

        if (billBll.checkoutTable(tableId)) {
            cout << ">> Da thanh toan.\n";
        } else {
            cout << ">> Khong co bill dang mo de thanh toan.\n";
        }
    } catch (const exception &e) {
        cout << "Loi thanh toan: " << e.what() << "\n";
    }
}

void StaffUI::tableOrderMenu(int tableId) {
    while (true) {
        cout << "\n=== Quan ly order cho BAN #" << tableId << " ===\n";
        cout << "1. Xem order hien tai\n";
        cout << "2. Them mon\n";
        cout << "3. Cap nhat mon\n";
        cout << "4. Xoa mon\n";
        cout << "5. Thanh toan\n";
        cout << "0. Quay lai\n";
        cout << "Chon: ";
        int c;
        if (!(cin >> c))
            return;
        cin.ignore();

        if (c == 1)
            showCurrentOrder(tableId);
        else if (c == 2)
            addFoodFlow(tableId);
        else if (c == 3)
            updateItemFlow(tableId);
        else if (c == 4)
            deleteItemFlow(tableId);
        else if (c == 5) {
            checkoutFlow(tableId);
            break;
        } else if (c == 0)
            break;
        else
            cout << "Lua chon khong hop le\n";
    }
}

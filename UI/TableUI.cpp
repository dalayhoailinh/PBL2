#include "TableUI.h"
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

void TableUI::printTableHeader() {
    cout << string(45, '=') << endl;
    cout << left << setw(6) << "ID"
         << setw(12) << "So ban"
         << setw(15) << "So cho ngoi"
         << setw(15) << "Trang thai"
         << endl;
    cout << string(45, '-') << endl;
}

void TableUI::printTableRow(const Table &t) {
    string statusText = (t.status_id == 0) ? "Trong" : "Co khach";
    cout << left << setw(6) << t.id
         << setw(12) << t.number
         << setw(15) << t.capacity
         << setw(15) << statusText << endl;
}

void TableUI::listTables() {
    try {
        vector<Table> rows = tableBll.getAll();
        cout << "\n=== DANH SACH BAN AN ===\n";
        if (rows.empty()) {
            cout << "Khong co ban nao trong he thong.\n";
            return;
        }
        printTableHeader();
        for (size_t i = 0; i < rows.size(); ++i)
            printTableRow(rows[i]);
        cout << string(45, '=') << endl;
        cout << "Tong cong: " << rows.size() << " ban\n";
    } catch (const exception &e) {
        cout << "Loi khi lay danh sach ban: " << e.what() << endl;
    }
}

void TableUI::addTable() {
    cout << "\n=== THEM BAN MOI ===\n";
    Table t;
    t.id = 0;
    t.number = 0;
    t.capacity = 0;
    t.status_id = 0;

    cout << "Nhap so ban: ";
    cin >> t.number;
    cin.ignore();

    cout << "Nhap so cho ngoi: ";
    cin >> t.capacity;
    cin.ignore();

    Result r = tableBll.addChecked(t);
    cout << r.message << "\n";
}

void TableUI::editTable() {
    int id;
    cout << "\n=== SUA THONG TIN BAN ===\n";
    cout << "Nhap table_id can sua: ";
    cin >> id;
    cin.ignore();

    Table t;
    if (!tableBll.getById(id, t)) {
        cout << "Khong tim thay ban co ID = " << id << "\n";
        return;
    }

    cout << "\nThong tin ban hien tai:\n";
    printTableHeader();
    printTableRow(t);
    cout << string(45, '-') << endl;

    cout << "Nhap so ban moi (" << t.number << "): ";
    string input;
    getline(cin, input);
    if (!input.empty()) {
        t.number = atoi(input.c_str());
    }

    cout << "Nhap so cho ngoi moi (" << t.capacity << "): ";
    getline(cin, input);
    if (!input.empty()) {
        t.capacity = atoi(input.c_str());
    }

    Result r = tableBll.updateChecked(t);
    cout << r.message << "\n";
}

void TableUI::deleteTable() {
    int id;
    cout << "\n=== XOA BAN ===\n";
    cout << "Nhap table_id can xoa: ";
    cin >> id;
    cin.ignore();

    Table t;
    if (!tableBll.getById(id, t)) {
        cout << "Khong tim thay ban co ID = " << id << "\n";
        return;
    }

    cout << "\nThong tin ban se bi xoa:\n";
    printTableHeader();
    printTableRow(t);
    cout << string(45, '-') << endl;

    cout << "Ban co chac chan muon xoa ban nay? (y/n): ";
    char confirm;
    cin >> confirm;
    cin.ignore();

    if (confirm == 'y' || confirm == 'Y') {
        Result r = tableBll.removeChecked(id);
        cout << r.message << "\n";
    } else {
        cout << "Da huy thao tac xoa ban.\n";
    }
}

void TableUI::menu() {
    while (true) {
        cout << "\n=== Quan ly ban an===\n";
        cout << "1. Xem danh sach ban\n";
        cout << "2. Them ban\n";
        cout << "3. Sua ban\n";
        cout << "4. Xoa ban\n";
        cout << "0. Quay lai\n";
        cout << "Chon: ";

        int c;
        if (!(cin >> c))
            return;
        cin.ignore();

        if (c == 1)
            listTables();
        else if (c == 2)
            addTable();
        else if (c == 3)
            editTable();
        else if (c == 4)
            deleteTable();
        else if (c == 0)
            break;
        else
            cout << "Lua chon khong hop le\n";
    }
}

#include "MainUI.h"
#include <iostream>
using namespace std;

void MainUI::menu() {
    while (true) {
        cout << "\n=== He thong quan ly nha hang ===\n";
        cout << "1. Quan ly mon an\n";
        cout << "0. Thoat\n";
        cout << "Chon: ";
        int c;
        if (!(cin >> c)) return;
        cin.ignore();

        if (c == 1)      foodUI.menu();
        else if (c == 0) break;
        else             cout << "Lua chon khong hop le\n";
    }
}


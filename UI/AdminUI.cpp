#include "AdminUI.h"
#include <iostream>
using namespace std;

void AdminUI::menu() {
    while (true){
        cout << "\n=== Admin Menu ===\n";
        cout << "1. Quan ly mon an\n";
        cout << "2. Quan ly danh muc\n";
        cout << "3. Quan ly nhan vien\n";
        cout << "4. Chuyen sang giao dien Nhan vien\n";
        cout << "0. Dang xuat\n";
        cout << "Chon: ";
        int c;
        if (!(cin >> c)) return;
        cin.ignore();

        if (c==1){
        	foodUi.menu();
		}
        else if (c==2) cout << "Quan ly danh muc (dang phat trien)\n";
        else if (c==3) userUi.menu();
        else if (c==4) staffUi.menu(true);
        else if (c==0) {
        	userBll.logOut();
        	break;
		}
        else cout << "Lua chon khong hop le\n";
    }
}


#include "StaffUI.h"
#include <iostream>
using namespace std;

void StaffUI::menu(bool isAdminView) {
    while (true){
        cout << "\n=== Staff Menu ===\n";
        cout << "1. Xem danh sach mon an\n";
        cout << "2. Xem thong tin ca nhan\n";
        if (isAdminView) cout << "3. Chuyen sang giao dien Admin\n";
        cout << "0. Dang xuat\n";
        cout << "Chon: ";
        int c;
        if (!(cin >> c)) return;
        cin.ignore();

        if (c==1) cout << "Xem mon an (dang phat trien)\n";
        else if (c==2) {
            userUi.profileMenu();
        }
        else if (isAdminView && c==3) return;
        else if (c==0){
        	userBll.logOut();
        	break;
		}
        else cout << "Lua chon khong hop le\n";
    }
}


#include "UserUI.h"
#include <iostream>
#include <iomanip>
using namespace std;

void UserUI::loginMenu(){
    string uname, pwd;
    cout << "Nhap username: ";
    getline(cin, uname);
    cout << "Nhap password: ";
    getline(cin, pwd);

    Result r = userBll.login(uname, pwd);
    cout << r.message << "\n";
}

void UserUI::profileMenu(){
    User u = userBll.getCurrentUser();
    if (u.id == 0){
        cout << "Chua dang nhap\n";
        return;
    }

    cout << "\n=== Thong tin ca nhan ===\n";
    cout << "ID: " << u.id << "\n";
    cout << "Username: " << u.userName << "\n";
    cout << "Ho ten: " << u.fullName << "\n";
    cout << "So dien thoai: " << u.phoneNumber << "\n";
    cout << "Ngay sinh: " << u.birth << "\n";
    cout << "Gioi tinh: " << userBll.genderToString(u.genderId) << "\n";
    cout << "Vai tro: "   << userBll.roleToString(u.roleId)   << "\n";
}

void UserUI::menu(){
    if (userBll.getCurrentUser().roleId != 1){
        cout << "Khong co quyen truy cap chuc nang nay\n";
        return;
    }

    while (true){
        cout << "\n=== Quan ly nhan vien ===\n";
        cout << "1. Danh sach nhan vien\n";
        cout << "2. Them nhan vien\n";
        cout << "3. Sua nhan vien\n";
        cout << "4. Xoa nhan vien\n";
        cout << "0. Quay lai\n";
        cout << "Chon: ";
        int c;
        if (!(cin >> c)) return;
        cin.ignore();

        if (c==1) listUsers();
        else if (c==2) addUser();
        else if (c==3) editUser();
        else if (c==4) deleteUser();
        else if (c==0) break;
        else cout << "Lua chon khong hop le\n";
    }
}

void UserUI::listUsers(){
    vector<User> rows = userBll.getAll();
    if (rows.empty()){
        cout << "(Khong co du lieu hoac khong co quyen)\n";
        return;
    }
    cout << left << setw(6)  << "ID"
         << setw(16) << "Username"
         << setw(22) << "Ho ten"
         << setw(16) << "Phone"
         << setw(12) << "Birth"
         << setw(8)  << "Gender"
         << "Role\n";
    for (size_t i=0;i<rows.size();++i){
        const User& u = rows[i];
        cout << left << setw(6)  << u.id
             << setw(16) << u.userName
             << setw(22) << u.fullName
             << setw(16) << u.phoneNumber
             << setw(12) << u.birth
             << setw(8)  << userBll.genderToString(u.genderId)
             << userBll.roleToString(u.roleId) << "\n";
    }
}

void UserUI::addUser(){
    User u; u.id=0;
    cout << "Nhap username: ";     getline(cin, u.userName);
    cout << "Nhap password: ";     getline(cin, u.password);
    cout << "Nhap ho ten: ";       getline(cin, u.fullName);
    cout << "Nhap so dien thoai: ";getline(cin, u.phoneNumber);
    cout << "Nhap ngay sinh (yyyy-mm-dd): "; getline(cin, u.birth);
    cout << "Nhap gender (1=nam,0=nu): ";    cin >> u.genderId; cin.ignore();
    cout << "Nhap role   (1=Admin,0=NV): ";  cin >> u.roleId;   cin.ignore();

    Result r = userBll.addChecked(u);
    cout << r.message << "\n";
}

void UserUI::editUser(){
    int id; cout << "Nhap user_id can sua: "; cin >> id; cin.ignore();
    User u;
    if (!userBll.getById(id, u)){ cout << "Khong tim thay hoac khong co quyen\n"; return; }

    cout << "Username hien tai: " << u.userName << "\nNhap username moi (de trong = giu): ";
    string s; getline(cin, s); if (!s.empty()) u.userName = s;

    cout << "Password hien tai: (an)\nNhap password moi (de trong = giu): ";
    getline(cin, s); if (!s.empty()) u.password = s;

    cout << "Ho ten hien tai: " << u.fullName << "\nNhap ho ten moi (de trong = giu): ";
    getline(cin, s); if (!s.empty()) u.fullName = s;

    cout << "Phone hien tai: " << u.phoneNumber << "\nNhap phone moi (de trong = giu): ";
    getline(cin, s); if (!s.empty()) u.phoneNumber = s;

    cout << "Birth hien tai: " << u.birth << "\nNhap birth moi yyyy-mm-dd (de trong = giu): ";
    getline(cin, s); if (!s.empty()) u.birth = s;

    cout << "Gender hien tai: " << userBll.genderToString(u.genderId)
         << "\nNhap gender moi (1=nam,0=nu, -1=giu): ";
    int gi; cin >> gi; cin.ignore(); if (gi==0 || gi==1) u.genderId = gi;

    cout << "Role hien tai: " << userBll.roleToString(u.roleId)
         << "\nNhap role moi (1=Admin,0=NV, -1=giu): ";
    int ri; cin >> ri; cin.ignore(); if (ri==0 || ri==1) u.roleId = ri;

    Result r = userBll.updateChecked(u);
    cout << r.message << "\n";
}

void UserUI::deleteUser(){
    int id; cout << "Nhap user_id can xoa: "; cin >> id; cin.ignore();
    Result r = userBll.removeChecked(id);
    cout << r.message << "\n";
}

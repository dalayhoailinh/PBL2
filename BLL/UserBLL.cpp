#include "UserBLL.h"
#include "../UI/ConsoleUI.h"
using namespace std;

Result validateUserName(const string& s){
    Result r{false, "Username khong hop le"};
    if (s.empty()) { r.message = "Username khong duoc rong"; return r; }
    if (s.size() > 50) { r.message = "Username qua dai"; return r; }
    r.ok = true; r.message = ""; return r;
}
Result validatePassword(const string& s){
    Result r{false, "Password khong hop le"};
    if (s.empty()) { r.message = "Password khong duoc rong"; return r; }
    if (s.size() > 50) { r.message = "Password qua dai"; return r; }
    r.ok = true; r.message = ""; return r;
}
Result validateRole(int roleId){
    Result r{false, "Role khong hop le"};
    if (roleId!=0 && roleId!=1){ r.message="Role chi nhan 0 hoac 1"; return r; }
    r.ok=true; r.message=""; return r;
}
Result validateGender(int genderId){
    Result r{false, "Gender khong hop le"};
    if (genderId!=0 && genderId!=1){ r.message="Gender chi nhan 0 hoac 1"; return r; }
    r.ok=true; r.message=""; return r;
}

Result UserBLL::login(const string& username, const string& password){
    Result r;
    currentUser = User();
    User u;
    if (username.empty() && password.empty()){
    	currentUser.id = -1;
    	r.ok = true;
        r.message = "";
    	return r;
	}
    if (userDal.login(username, password, u)){
        currentUser = u;
        r.ok = true;
        r.message = "Dang nhap thanh cong";
    } else {
        r.ok = false;
        r.message = "Sai ten dang nhap hoac mat khau";
    }
    return r;
}

User UserBLL::getCurrentUser(){
    return currentUser;
}

string UserBLL::genderToString(int genderId){
    return genderId == 1 ? "Nam" : "Nu";
}

string UserBLL::roleToString(int roleId){
    return roleId == 1 ? "Admin" : "Nhan vien";
}

void UserBLL::logOut(){
	currentUser = User();
	ConsoleUI::clearAll();
}

vector<User> UserBLL::getAll(){
    if (currentUser.roleId != 1) return {};
    return userDal.getAll();
}

bool UserBLL::getById(int id, User& out){
    if (currentUser.roleId != 1) return false;
    return userDal.getById(id, out);
}

Result UserBLL::addChecked(const User& u){
    Result r;
    if (currentUser.roleId != 1){ r.ok=false; r.message="Khong co quyen"; return r; }

    r = validateUserName(u.userName); if(!r.ok) return r;
    r = validatePassword(u.password); if(!r.ok) return r;
    r = validateGender(u.genderId);   if(!r.ok) return r;
    r = validateRole(u.roleId);       if(!r.ok) return r;

    bool ok = userDal.insert(u);
    r.ok = ok; r.message = ok ? "Them nhan vien thanh cong" : "Them nhan vien that bai";
    return r;
}

Result UserBLL::updateChecked(const User& u){
    Result r;
    if (currentUser.roleId != 1){ r.ok=false; r.message="Khong co quyen"; return r; }

    r = validateUserName(u.userName); if(!r.ok) return r;
    r = validatePassword(u.password); if(!r.ok) return r;
    r = validateGender(u.genderId);   if(!r.ok) return r;
    r = validateRole(u.roleId);       if(!r.ok) return r;

    bool ok = userDal.update(u);
    r.ok = ok; r.message = ok ? "Cap nhat nhan vien thanh cong" : "Cap nhat nhan vien that bai";
    return r;
}

Result UserBLL::removeChecked(int id){
    Result r;
    if (currentUser.roleId != 1){ r.ok=false; r.message="Khong co quyen"; return r; }

    bool ok = userDal.remove(id);
    r.ok = ok; r.message = ok ? "Xoa nhan vien thanh cong" : "Xoa nhan vien that bai";
    return r;
}

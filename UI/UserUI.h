#ifndef USER_UI_H
#define USER_UI_H
#include "../BLL/UserBLL.h"
#include "../BLL/Result.h"
using namespace std;

class UserUI {
	private:
	    UserBLL& userBll;
	    void listUsers();
	    void addUser();
	    void editUser();
	    void deleteUser();
	    
	public:
	    UserUI(UserBLL& uBll) : userBll(uBll) {}
	    void loginMenu();
	    void profileMenu();
	    void menu();
};
#endif


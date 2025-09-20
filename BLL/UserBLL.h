#ifndef USER_BLL_H
#define USER_BLL_H
#include "../DAL/UserDAL.h"
#include "../Models/User.h"
#include "Result.h"
#include <string>
#include <vector>
using namespace std;

class UserBLL {
	private:
	    UserDAL& userDal;
	    User currentUser;
	
	public:
	    UserBLL(UserDAL& uDal) : userDal(uDal) {
			currentUser = User();
		}
	    Result login(const string& username, const string& password);
	    User getCurrentUser();
	
	    string genderToString(int genderId);
	    string roleToString(int roleId);
	    void logOut();
	    	    
	    // CRUD
	    vector<User> getAll();
	    bool getById(int id, User& out);
	    Result addChecked(const User& u);
	    Result updateChecked(const User& u);
	    Result removeChecked(int id);
};
#endif


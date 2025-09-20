#ifndef USER_DAL_H
#define USER_DAL_H
#include "DatabaseConnection.h"
#include "../Models/User.h"
#include <vector>

class UserDAL {
	public:
	    bool login(const string& userName, const string& password, User& out);
	    
	    // CRUD
	    vector<User> getAll();
	    bool getById(int id, User& out);
	    bool insert(const User& u);
	    bool update(const User& u);
	    bool remove(int id);
};
#endif


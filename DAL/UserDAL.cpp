#include "UserDAL.h"
#include <sql.h>
#include <sqlext.h>
#include <stdexcept>
#include <string>
#include <cstring>
using namespace std;

static void throwStmtErr(SQLHANDLE stmt, const char* msg){
    SQLCHAR state[6], text[SQL_MAX_MESSAGE_LENGTH]; SQLINTEGER native; SQLSMALLINT len;
    SQLGetDiagRec(SQL_HANDLE_STMT, stmt, 1, state, &native, text, sizeof(text), &len);
    throw runtime_error(string(msg) + " - " + (char*)text);
}

bool UserDAL::login(const string& userName, const string& password, User& out){
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char* q =
        "SELECT user_id, user_name, password, full_name, phone_number, "
        "birth, gender_id, role_id "
        "FROM users WHERE user_name = ? AND password = ?";

    if (SQLPrepare(stmt, (SQLCHAR*)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare login fail");

    SQLCHAR uname[50], pwd[50];
    snprintf((char*)uname, sizeof(uname), "%s", userName.c_str());
    snprintf((char*)pwd, sizeof(pwd), "%s", password.c_str());

    SQLLEN unameLen = SQL_NTS, pwdLen = SQL_NTS;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50,0, uname, 0, &unameLen);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50,0, pwd, 0, &pwdLen);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return false;
    }

    SQLINTEGER id, roleId, genderId;
    char unameOut[50], fullName[100], phone[20], birth[20];
    SQLLEN idLen, unameOutLen, fullNameLen, phoneLen, birthLen, genderLen, roleLen;

    SQLBindCol(stmt, 1, SQL_C_LONG, &id, 0, &idLen);
    SQLBindCol(stmt, 2, SQL_C_CHAR, unameOut, sizeof(unameOut), &unameOutLen);
    SQLBindCol(stmt, 4, SQL_C_CHAR, fullName, sizeof(fullName), &fullNameLen);
    SQLBindCol(stmt, 5, SQL_C_CHAR, phone, sizeof(phone), &phoneLen);
    SQLBindCol(stmt, 6, SQL_C_CHAR, birth, sizeof(birth), &birthLen);
    SQLBindCol(stmt, 7, SQL_C_LONG, &genderId, 0, &genderLen);
    SQLBindCol(stmt, 8, SQL_C_LONG, &roleId, 0, &roleLen);

    if (SQLFetch(stmt) == SQL_SUCCESS){
        out.id = id;
        out.userName = string(unameOut);
        out.fullName = string(fullName);
        out.phoneNumber = string(phone);
        out.birth = string(birth);
        out.genderId = genderId;
        out.roleId = roleId;

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return true;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return false;
}

vector<User> UserDAL::getAll(){
    vector<User> items;
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char* q =
    "SELECT user_id, user_name, password, full_name, phone_number, birth, gender_id, role_id "
    "FROM users WHERE role_id = 0 ORDER BY user_id";
    if (SQLExecDirect(stmt, (SQLCHAR*)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "select users fail");

    SQLINTEGER id, genderId, roleId;
    char userName[64], password[64], fullName[128], phone[32], birth[32];
    SQLLEN idLen, unLen, pwLen, fnLen, phLen, bLen, gLen, rLen;

    SQLBindCol(stmt, 1, SQL_C_LONG,   &id, 0, &idLen);
    SQLBindCol(stmt, 2, SQL_C_CHAR,   userName, sizeof(userName), &unLen);
    SQLBindCol(stmt, 3, SQL_C_CHAR,   password, sizeof(password), &pwLen);
    SQLBindCol(stmt, 4, SQL_C_CHAR,   fullName, sizeof(fullName), &fnLen);
    SQLBindCol(stmt, 5, SQL_C_CHAR,   phone, sizeof(phone), &phLen);
    SQLBindCol(stmt, 6, SQL_C_CHAR,   birth, sizeof(birth), &bLen);
    SQLBindCol(stmt, 7, SQL_C_LONG,   &genderId, 0, &gLen);
    SQLBindCol(stmt, 8, SQL_C_LONG,   &roleId,   0, &rLen);

    while (SQLFetch(stmt) == SQL_SUCCESS){
        User u;
        u.id         = id;
        u.userName   = unLen == SQL_NULL_DATA ? "" : string(userName);
        u.password   = pwLen == SQL_NULL_DATA ? "" : string(password);
        u.fullName   = fnLen == SQL_NULL_DATA ? "" : string(fullName);
        u.phoneNumber= phLen == SQL_NULL_DATA ? "" : string(phone);
        u.birth      = bLen == SQL_NULL_DATA ? "" : string(birth);
        u.genderId   = genderId;
        u.roleId     = roleId;
        items.push_back(u);
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return items;
}

bool UserDAL::getById(int idQuery, User& out){
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char* q =
        "SELECT user_id, user_name, password, full_name, phone_number, birth, gender_id, role_id "
        "FROM users WHERE user_id = ?";
    if (SQLPrepare(stmt, (SQLCHAR*)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare get user fail");

    SQLINTEGER id = idQuery; SQLLEN idInd = 0;
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &id, 0, &idInd);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return false;
    }

    SQLINTEGER genderId, roleId;
    char userName[64], password[64], fullName[128], phone[32], birth[32];
    SQLLEN idLen, unLen, pwLen, fnLen, phLen, bLen, gLen, rLen;

    SQLBindCol(stmt, 1, SQL_C_LONG,   &id, 0, &idLen);
    SQLBindCol(stmt, 2, SQL_C_CHAR,   userName, sizeof(userName), &unLen);
    SQLBindCol(stmt, 3, SQL_C_CHAR,   password, sizeof(password), &pwLen);
    SQLBindCol(stmt, 4, SQL_C_CHAR,   fullName, sizeof(fullName), &fnLen);
    SQLBindCol(stmt, 5, SQL_C_CHAR,   phone, sizeof(phone), &phLen);
    SQLBindCol(stmt, 6, SQL_C_CHAR,   birth, sizeof(birth), &bLen);
    SQLBindCol(stmt, 7, SQL_C_LONG,   &genderId, 0, &gLen);
    SQLBindCol(stmt, 8, SQL_C_LONG,   &roleId,   0, &rLen);

    if (SQLFetch(stmt) == SQL_SUCCESS){
        out.id = id; out.userName = string(userName); out.password = string(password);
        out.fullName = string(fullName); out.phoneNumber = string(phone);
        out.birth = string(birth); out.genderId = genderId; out.roleId = roleId;
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return true;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return false;
}

bool UserDAL::insert(const User& u){
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char* q =
        "INSERT INTO users (user_name, password, full_name, phone_number, birth, gender_id, role_id) "
        "VALUES (?, ?, ?, ?, ?, ?, ?)";
    if (SQLPrepare(stmt, (SQLCHAR*)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare insert user fail");

    SQLCHAR un[64], pw[64], fn[128], ph[32], br[32];
    snprintf((char*)un, sizeof(un), "%s", u.userName.c_str());
    snprintf((char*)pw, sizeof(pw), "%s", u.password.c_str());
    snprintf((char*)fn, sizeof(fn), "%s", u.fullName.c_str());
    snprintf((char*)ph, sizeof(ph), "%s", u.phoneNumber.c_str());
    snprintf((char*)br, sizeof(br), "%s", u.birth.c_str());

    SQLINTEGER genderId = u.genderId, roleId = u.roleId;
    SQLLEN unLen=SQL_NTS, pwLen=SQL_NTS, fnLen=SQL_NTS, phLen=SQL_NTS, brLen=SQL_NTS, gLen=0, rLen=0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50,0, un, 0, &unLen);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50,0, pw, 0, &pwLen);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,100,0, fn, 0, &fnLen);
    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20,0, ph, 0, &phLen);
    SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20,0, br, 0, &brLen);
    SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &genderId, 0, &gLen);
    SQLBindParameter(stmt, 7, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &roleId,   0, &rLen);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt, "execute insert user fail");

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return true;
}

bool UserDAL::update(const User& u){
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char* q =
        "UPDATE users SET user_name=?, password=?, full_name=?, phone_number=?, birth=?, gender_id=?, role_id=? "
        "WHERE user_id=?";
    if (SQLPrepare(stmt, (SQLCHAR*)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare update user fail");

    SQLCHAR un[64], pw[64], fn[128], ph[32], br[32];
    snprintf((char*)un, sizeof(un), "%s", u.userName.c_str());
    snprintf((char*)pw, sizeof(pw), "%s", u.password.c_str());
    snprintf((char*)fn, sizeof(fn), "%s", u.fullName.c_str());
    snprintf((char*)ph, sizeof(ph), "%s", u.phoneNumber.c_str());
    snprintf((char*)br, sizeof(br), "%s", u.birth.c_str());

    SQLINTEGER genderId = u.genderId, roleId = u.roleId, id = u.id;
    SQLLEN unLen=SQL_NTS, pwLen=SQL_NTS, fnLen=SQL_NTS, phLen=SQL_NTS, brLen=SQL_NTS, gLen=0, rLen=0, idLen=0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50,0, un, 0, &unLen);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50,0, pw, 0, &pwLen);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,100,0, fn, 0, &fnLen);
    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20,0, ph, 0, &phLen);
    SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20,0, br, 0, &brLen);
    SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &genderId, 0, &gLen);
    SQLBindParameter(stmt, 7, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &roleId,   0, &rLen);
    SQLBindParameter(stmt, 8, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &id,       0, &idLen);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt, "execute update user fail");

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return true;
}

bool UserDAL::remove(int idQuery){
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char* q = "DELETE FROM users WHERE user_id = ?";
    if (SQLPrepare(stmt, (SQLCHAR*)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare delete user fail");

    SQLINTEGER id = idQuery; SQLLEN idLen=0;
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &id, 0, &idLen);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt, "execute delete user fail");

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return true;
}


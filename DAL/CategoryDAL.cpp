#include "CategoryDAL.h"
#include <sql.h>
#include <sqlext.h>
#include <stdexcept>
#include <string>
using namespace std;

static void throwStmtErr(SQLHANDLE stmt, const char* msg){
    SQLCHAR state[6], text[SQL_MAX_MESSAGE_LENGTH]; SQLINTEGER native; SQLSMALLINT len;
    SQLGetDiagRec(SQL_HANDLE_STMT, stmt, 1, state, &native, text, sizeof(text), &len);
    throw runtime_error(string(msg) + " - " + (char*)text);
}

vector<Category> CategoryDAL::getAll(){
    vector<Category> items;
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char* q = "SELECT category_id, category_name FROM categories ORDER BY category_id";
    if (SQLExecDirect(stmt, (SQLCHAR*)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "select categories fail");

    SQLINTEGER id; char name[256];
    SQLLEN idLen, nameLen;

    SQLBindCol(stmt, 1, SQL_C_LONG, &id, 0, &idLen);
    SQLBindCol(stmt, 2, SQL_C_CHAR, name, sizeof(name), &nameLen);

    while (SQLFetch(stmt) == SQL_SUCCESS){
        Category c;
        c.id = id;
        c.name = nameLen == SQL_NULL_DATA ? "" : string(name);
        items.push_back(c);
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return items;
}

bool CategoryDAL::getById(int idQuery, Category& out){
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char* q = "SELECT category_id, category_name FROM categories WHERE category_id = ?";
    if (SQLPrepare(stmt, (SQLCHAR*)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare fail");

    SQLINTEGER id = idQuery; SQLLEN idInd = 0;
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &id, 0, &idInd);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return false;
    }

    SQLINTEGER rid; char name[256]; SQLLEN ridLen, nameLen;
    SQLBindCol(stmt, 1, SQL_C_LONG, &rid, 0, &ridLen);
    SQLBindCol(stmt, 2, SQL_C_CHAR, name, sizeof(name), &nameLen);

    if (SQLFetch(stmt) == SQL_SUCCESS){
        out.id = rid;
        out.name = nameLen == SQL_NULL_DATA ? "" : string(name);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return true;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return false;
}


#include "FoodDAL.h"
#include <sql.h>
#include <sqlext.h>
#include <stdexcept>
#include <cstring>
#include <string>
using namespace std;

static void throwStmtErr(SQLHANDLE stmt, const char* msg){
    SQLCHAR state[6], text[SQL_MAX_MESSAGE_LENGTH]; SQLINTEGER native; SQLSMALLINT len;
    SQLGetDiagRec(SQL_HANDLE_STMT, stmt, 1, state, &native, text, sizeof(text), &len);
    throw runtime_error(string(msg) + " - " + (char*)text);
}

vector<Food> FoodDAL::getAll(){
    vector<Food> items;
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char* q =
        "SELECT f.food_id, f.food_name, f.category_id, c.category_name, f.price "
        "FROM foods f INNER JOIN categories c ON f.category_id = c.category_id "
        "ORDER BY f.food_id";
    if (SQLExecDirect(stmt, (SQLCHAR*)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "select foods fail");

    SQLINTEGER id, categoryId; char name[256], categoryName[256]; double price;
    SQLLEN idLen, nameLen, categoryIdLen, categoryNameLen, priceLen;

    SQLBindCol(stmt, 1, SQL_C_LONG,   &id, 0, &idLen);
    SQLBindCol(stmt, 2, SQL_C_CHAR,   name, sizeof(name), &nameLen);
    SQLBindCol(stmt, 3, SQL_C_LONG,   &categoryId, 0, &categoryIdLen);
    SQLBindCol(stmt, 4, SQL_C_CHAR,   categoryName, sizeof(categoryName), &categoryNameLen);
    SQLBindCol(stmt, 5, SQL_C_DOUBLE, &price, 0, &priceLen);

    while (SQLFetch(stmt) == SQL_SUCCESS){
        Food f;
        f.id = id;
        f.name = nameLen == SQL_NULL_DATA ? "" : string(name);
        f.categoryId = categoryId;
        f.categoryName = categoryNameLen == SQL_NULL_DATA ? "" : string(categoryName);
        f.price = price;
        items.push_back(f);
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return items;
}

bool FoodDAL::getById(int idQuery, Food& out){
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char* q =
        "SELECT f.food_id, f.food_name, f.category_id, c.category_name, f.price "
        "FROM foods f INNER JOIN categories c ON f.category_id = c.category_id "
        "WHERE f.food_id = ?";
    if (SQLPrepare(stmt, (SQLCHAR*)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare get fail");

    SQLINTEGER id = idQuery; SQLLEN idLen = 0;
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &id, 0, &idLen);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return false;
    }

    SQLINTEGER fid, categoryId; char name[256], categoryName[256]; double price;
    SQLLEN fidLen, nameLen, categoryIdLen, categoryNameLen, priceLen;

    SQLBindCol(stmt, 1, SQL_C_LONG,   &fid, 0, &fidLen);
    SQLBindCol(stmt, 2, SQL_C_CHAR,   name, sizeof(name), &nameLen);
    SQLBindCol(stmt, 3, SQL_C_LONG,   &categoryId, 0, &categoryIdLen);
    SQLBindCol(stmt, 4, SQL_C_CHAR,   categoryName, sizeof(categoryName), &categoryNameLen);
    SQLBindCol(stmt, 5, SQL_C_DOUBLE, &price, 0, &priceLen);

    if (SQLFetch(stmt) == SQL_SUCCESS){
        out.id = fid;
        out.name = string(name);
        out.categoryId = categoryId;
        out.categoryName = string(categoryName);
        out.price = price;
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return true;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return false;
}

bool FoodDAL::insert(const Food& food){
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char* q = "INSERT INTO foods (food_name, category_id, price) VALUES (?, ?, ?)";
    if (SQLPrepare(stmt, (SQLCHAR*)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare insert fail");

    SQLCHAR name[256]; snprintf((char*)name, sizeof(name), "%s", food.name.c_str());
    SQLINTEGER categoryId = food.categoryId; double price = food.price;
    SQLLEN nameLen = SQL_NTS, categoryLen = 0, priceLen = 0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR,   SQL_VARCHAR, 255,0, name, 0, &nameLen);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_LONG,   SQL_INTEGER, 0,  0, &categoryId, 0, &categoryLen);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE,  0,  0, &price,     0, &priceLen);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt, "execute insert fail");

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return true;
}

bool FoodDAL::update(const Food& food){
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char* q = "UPDATE foods SET food_name = ?, category_id = ?, price = ? WHERE food_id = ?";
    if (SQLPrepare(stmt, (SQLCHAR*)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare update fail");

    SQLCHAR name[256]; snprintf((char*)name, sizeof(name), "%s", food.name.c_str());
    SQLINTEGER categoryId = food.categoryId; double price = food.price; SQLINTEGER id = food.id;
    SQLLEN nameLen = SQL_NTS, categoryLen=0, priceLen=0, idLen=0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR,   SQL_VARCHAR, 255,0, name,      0, &nameLen);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_LONG,   SQL_INTEGER, 0,    0, &categoryId, 0, &categoryLen);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE,  0,    0, &price,   0, &priceLen);
    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_LONG,   SQL_INTEGER, 0,    0, &id,      0, &idLen);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt, "execute update fail");

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return true;
}

bool FoodDAL::remove(int idQuery){
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char* q = "DELETE FROM foods WHERE food_id = ?";
    if (SQLPrepare(stmt, (SQLCHAR*)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare delete fail");

    SQLINTEGER id = idQuery; SQLLEN idLen=0;
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &id, 0, &idLen);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt, "execute delete fail");

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return true;
}


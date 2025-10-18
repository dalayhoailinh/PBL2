#include "TableDAL.h"
#include <sql.h>
#include <sqlext.h>
#include <stdexcept>
#include <cstring>
#include <string>
#include <iostream>
using namespace std;

static void throwStmtErr(SQLHANDLE stmt, const char* msg) {
	SQLCHAR state[6], text[SQL_MAX_MESSAGE_LENGTH];
	SQLINTEGER native;
	SQLSMALLINT len;
	SQLGetDiagRec(SQL_HANDLE_STMT, stmt, 1, state, &native, text, sizeof(text), &len);
	throw runtime_error(string(msg) + " - " + (char*)text);
}

vector<Table> TableDAL::getAll() {
	vector<Table> items;
	SQLHANDLE conn = DatabaseConnection::instance().getConnection();
	SQLHANDLE stmt;
	if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
		throw runtime_error("alloc stmt fail");

	const char* q =
	    "SELECT t.table_id, t.table_number, t.capacity, t.status_id "
	    "FROM tables t "
	    "ORDER BY t.table_id";
	if (SQLExecDirect(stmt, (SQLCHAR*)q, SQL_NTS) != SQL_SUCCESS)
		throwStmtErr(stmt, "select tables fail");

	SQLINTEGER id, number, capacity, statusId;
	SQLLEN idLen, numberLen, capacityLen, statusLen;

	SQLBindCol(stmt, 1, SQL_C_LONG, &id, 0, &idLen);
	SQLBindCol(stmt, 2, SQL_C_LONG, &number, 0, &numberLen);
	SQLBindCol(stmt, 3, SQL_C_LONG, &capacity, 0, &capacityLen);
	SQLBindCol(stmt, 4, SQL_C_LONG, &statusId, 0, &statusLen);

	while (SQLFetch(stmt) == SQL_SUCCESS) {
		Table t;
		t.id        = (idLen        == SQL_NULL_DATA) ? 0 : id;
		t.number    = (numberLen    == SQL_NULL_DATA) ? 0 : number;
		t.capacity  = (capacityLen  == SQL_NULL_DATA) ? 0 : capacity;
		t.status_id = (statusLen    == SQL_NULL_DATA) ? 0 : statusId;
		items.push_back(t);
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return items;
}

bool TableDAL::getById(int idQuery, Table& out) {
	SQLHANDLE conn = DatabaseConnection::instance().getConnection();
	SQLHANDLE stmt;
	if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
		throw runtime_error("alloc stmt fail");

	const char* q =
	    "SELECT t.table_id, t.table_number, t.capacity, t.status_id "
	    "FROM tables t "
	    "WHERE table_id = ?";
	if (SQLPrepare(stmt, (SQLCHAR*)q, SQL_NTS) != SQL_SUCCESS)
		throwStmtErr(stmt, "prepare get fail");

	SQLINTEGER id = idQuery;
	SQLLEN idLen = 0;
	SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &id, 0, &idLen);

	SQLRETURN rc = SQLExecute(stmt);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return false;
	}

	SQLINTEGER number, capacity, statusId;
	SQLLEN numberLen, capacityLen, statusLen;

	SQLBindCol(stmt, 1, SQL_C_LONG, &id, 0, &idLen);
	SQLBindCol(stmt, 2, SQL_C_LONG, &number, 0, &numberLen);
	SQLBindCol(stmt, 3, SQL_C_LONG, &capacity, 0, &capacityLen);
	SQLBindCol(stmt, 4, SQL_C_LONG, &statusId, 0, &statusLen);

	bool found = false;
	if (SQLFetch(stmt) == SQL_SUCCESS) {
		out.id        = id;
		out.number    = number;
		out.capacity  = capacity;
		out.status_id = (statusLen   == SQL_NULL_DATA) ? 0 : statusId;
		found = true;
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return found;
}

bool TableDAL::insert(const Table& table) {
	SQLHANDLE conn = DatabaseConnection::instance().getConnection();
	SQLHANDLE stmt;
	if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
		throw runtime_error("alloc stmt fail");

	const char* q = "INSERT INTO tables (table_number, capacity, status_id) VALUES (?, ?, ?)";
	if (SQLPrepare(stmt, (SQLCHAR*)q, SQL_NTS) != SQL_SUCCESS)
		throwStmtErr(stmt, "prepare insert table fail");

	SQLINTEGER number    = table.number;
	SQLINTEGER capacity  = table.capacity;
	SQLINTEGER status_id = table.status_id;
	SQLLEN numberLen=0, capacityLen=0, statusLen=0;

	SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &number, 0, &numberLen);
	SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &capacity, 0, &capacityLen);
	SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &status_id, 0, &statusLen);

	SQLRETURN rc = SQLExecute(stmt);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
		throwStmtErr(stmt, "execute insert table fail");

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return true;
}

bool TableDAL::update(const Table& table) {
	SQLHANDLE conn = DatabaseConnection::instance().getConnection();
	SQLHANDLE stmt;
	if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
		throw runtime_error("alloc stmt fail");

	const char* q = "UPDATE tables SET table_number=?, capacity=?, status_id=? WHERE table_id=?";
	if (SQLPrepare(stmt, (SQLCHAR*)q, SQL_NTS) != SQL_SUCCESS)
		throwStmtErr(stmt, "prepare update table fail");

	SQLINTEGER number    = table.number;
	SQLINTEGER capacity  = table.capacity;
	SQLINTEGER status_id = table.status_id;
	SQLINTEGER id        = table.id;
	SQLLEN numberLen=0, capacityLen=0, statusLen=0, idLen=0;

	SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &number, 0, &numberLen);
	SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &capacity, 0, &capacityLen);
	SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &status_id, 0, &statusLen);
	SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &id, 0, &idLen);

	SQLRETURN rc = SQLExecute(stmt);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
		throwStmtErr(stmt, "execute update table fail");

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return true;
}

bool TableDAL::remove(int idQuery) {
	SQLHANDLE conn = DatabaseConnection::instance().getConnection();
	SQLHANDLE stmt;
	if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
		throw runtime_error("alloc stmt fail");

	const char* q = "DELETE FROM tables WHERE table_id=?";
	if (SQLPrepare(stmt, (SQLCHAR*)q, SQL_NTS) != SQL_SUCCESS)
		throwStmtErr(stmt, "prepare delete table fail");

	SQLINTEGER id = idQuery;
	SQLLEN idLen=0;
	SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0, &id, 0, &idLen);

	SQLRETURN rc = SQLExecute(stmt);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
		throwStmtErr(stmt, "execute delete table fail");

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return true;
}

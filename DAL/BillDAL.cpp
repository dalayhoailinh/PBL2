#include "BillDAL.h"
#include <cstring>
#include <sql.h>
#include <sqlext.h>
#include <stdexcept>
#include <string>
using namespace std;

static void throwStmtErr(SQLHANDLE stmt, const char *msg) {
    SQLCHAR state[6], text[SQL_MAX_MESSAGE_LENGTH];
    SQLINTEGER native;
    SQLSMALLINT len;
    SQLGetDiagRec(SQL_HANDLE_STMT, stmt, 1, state, &native, text, sizeof(text), &len);
    throw runtime_error(string(msg) + " - " + (char *)text);
}

bool BillDAL::getOpenByTableId(int tableId, Bill &out) {
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char *q =
        "SELECT TOP 1 bill_id, table_id, total_price, paid_date "
        "FROM bills WHERE table_id=? AND paid_date IS NULL "
        "ORDER BY bill_id DESC";
    if (SQLPrepare(stmt, (SQLCHAR *)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare get open bill fail");

    SQLINTEGER t = tableId;
    SQLLEN tLen = 0;
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &t, 0, &tLen);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt, "execute get open bill fail");

    SQLINTEGER id, tbl;
    double total;
    SQLCHAR paid[32];
    SQLLEN paidInd = 0;

    SQLBindCol(stmt, 1, SQL_C_LONG, &id, 0, NULL);
    SQLBindCol(stmt, 2, SQL_C_LONG, &tbl, 0, NULL);
    SQLBindCol(stmt, 3, SQL_C_DOUBLE, &total, 0, NULL);
    SQLBindCol(stmt, 4, SQL_C_CHAR, paid, sizeof(paid), &paidInd);

    if (SQLFetch(stmt) == SQL_SUCCESS) {
        out.id = (int)id;
        out.table_id = (int)tbl;
        out.total_price = total;
        out.paid_date = (paidInd == SQL_NULL_DATA) ? "" : string((char *)paid);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return true;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return false;
}

int BillDAL::createForTable(int tableId) {
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char *q = "INSERT INTO bills (table_id, total_price, paid_date) VALUES (?, 0, NULL)";
    if (SQLPrepare(stmt, (SQLCHAR *)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare insert bill fail");

    SQLINTEGER t = tableId;
    SQLLEN tLen = 0;
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &t, 0, &tLen);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt, "execute insert bill fail");
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    Bill b;
    if (getOpenByTableId(tableId, b))
        return b.id;
    return -1;
}

bool BillDAL::deleteBill(int billId) {
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char *q = "DELETE FROM bills WHERE bill_id=?";
    if (SQLPrepare(stmt, (SQLCHAR *)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare delete bill fail");

    SQLINTEGER id = billId;
    SQLLEN idLen = 0;
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, &idLen);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt, "execute delete bill fail");
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return true;
}

bool BillDAL::recalcTotal(int billId) {
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char *q =
        "UPDATE bills SET total_price = "
        "  (SELECT COALESCE(SUM(sub_total),0) FROM bill_items WHERE bill_id=?) "
        "WHERE bill_id=?";
    if (SQLPrepare(stmt, (SQLCHAR *)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare recalc fail");

    SQLINTEGER id = billId;
    SQLLEN idLen = 0;
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, &idLen);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, &idLen);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt, "execute recalc fail");
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return true;
}

// --- helpers ---
bool BillDAL::getFoodPrice(int foodId, double &price) {
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char *q = "SELECT price FROM foods WHERE food_id=?";
    if (SQLPrepare(stmt, (SQLCHAR *)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare get food price fail");

    SQLINTEGER id = foodId;
    SQLLEN idLen = 0;
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, &idLen);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt, "execute get food price fail");

    double p = 0;
    SQLBindCol(stmt, 1, SQL_C_DOUBLE, &p, 0, NULL);
    if (SQLFetch(stmt) == SQL_SUCCESS) {
        price = p;
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return true;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return false;
}

// --- bill_items ---
bool BillDAL::addItem(int billId, int foodId, int quantity, const string &desc, int &outNewId) {
    double price = 0;
    if (!getFoodPrice(foodId, price))
        throw runtime_error("food not found");

    double sub = price * (quantity <= 0 ? 1 : quantity);

    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char *q =
        "INSERT INTO bill_items (bill_id, food_id, quantity, description, sub_total) "
        "VALUES (?, ?, ?, ?, ?)";
    if (SQLPrepare(stmt, (SQLCHAR *)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare insert item fail");

    SQLINTEGER b = billId, f = foodId, qtt = quantity;
    SQLLEN bL = 0, fL = 0, qL = 0, dL = SQL_NTS, sL = 0;
    SQLCHAR d[260];
    d[0] = '\0';
    if (!desc.empty()) {
        strncpy((char *)d, desc.c_str(), sizeof(d) - 1);
        d[sizeof(d) - 1] = '\0';
    }
    SQLDOUBLE subtotal = sub;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &b, 0, &bL);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &f, 0, &fL);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &qtt, 0, &qL);
    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, d, 0, &dL);
    SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &subtotal, 0, &sL);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt, "execute insert item fail");
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    SQLHANDLE stmt2;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt2) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");
    const char *q2 =
        "SELECT TOP 1 bill_item_id FROM bill_items "
        "WHERE bill_id=? AND food_id=? "
        "ORDER BY bill_item_id DESC";
    if (SQLPrepare(stmt2, (SQLCHAR *)q2, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt2, "prepare get new item id fail");
    SQLBindParameter(stmt2, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &b, 0, &bL);
    SQLBindParameter(stmt2, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &f, 0, &fL);
    if (SQLExecute(stmt2) != SQL_SUCCESS && SQLExecute(stmt2) != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt2, "execute get new item id fail");
    SQLINTEGER bi = 0;
    SQLBindCol(stmt2, 1, SQL_C_LONG, &bi, 0, NULL);
    if (SQLFetch(stmt2) == SQL_SUCCESS)
        outNewId = (int)bi;
    SQLFreeHandle(SQL_HANDLE_STMT, stmt2);

    return true;
}

bool BillDAL::updateItem(int billItemId, int quantity, const string &desc) {
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt0;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt0) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");
    const char *q0 = "SELECT food_id FROM bill_items WHERE bill_item_id=?";
    if (SQLPrepare(stmt0, (SQLCHAR *)q0, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt0, "prepare get item fail");
    SQLINTEGER iid = billItemId;
    SQLLEN iidL = 0;
    SQLBindParameter(stmt0, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &iid, 0, &iidL);
    if (SQLExecute(stmt0) != SQL_SUCCESS && SQLExecute(stmt0) != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt0, "execute get item fail");
    SQLINTEGER f = 0;
    SQLBindCol(stmt0, 1, SQL_C_LONG, &f, 0, NULL);
    if (SQLFetch(stmt0) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt0);
        return false;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt0);

    double price = 0;
    if (!getFoodPrice((int)f, price))
        throw runtime_error("food not found");
    double sub = price * (quantity <= 0 ? 1 : quantity);

    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");
    const char *q =
        "UPDATE bill_items SET quantity=?, description=?, sub_total=? "
        "WHERE bill_item_id=?";
    if (SQLPrepare(stmt, (SQLCHAR *)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare update item fail");

    SQLINTEGER qtt = quantity;
    SQLLEN qL = 0, dL = SQL_NTS, sL = 0, idL = 0;
    SQLCHAR d[260];
    d[0] = '\0';
    if (!desc.empty()) {
        strncpy((char *)d, desc.c_str(), sizeof(d) - 1);
        d[sizeof(d) - 1] = '\0';
    }
    SQLDOUBLE subtotal = sub;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &qtt, 0, &qL);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, d, 0, &dL);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &subtotal, 0, &sL);
    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &iid, 0, &idL);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt, "execute update item fail");
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return true;
}

bool BillDAL::removeItem(int billItemId) {
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char *q = "DELETE FROM bill_items WHERE bill_item_id=?";
    if (SQLPrepare(stmt, (SQLCHAR *)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare delete item fail");

    SQLINTEGER id = billItemId;
    SQLLEN idLen = 0;
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, &idLen);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt, "execute delete item fail");
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return true;
}

bool BillDAL::getItemContext(int billItemId, int &billId, int &tableId) {
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char *q =
        "SELECT bi.bill_id, b.table_id "
        "FROM bill_items bi INNER JOIN bills b ON bi.bill_id = b.bill_id "
        "WHERE bi.bill_item_id=?";
    if (SQLPrepare(stmt, (SQLCHAR *)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare context fail");

    SQLINTEGER iid = billItemId;
    SQLLEN iidL = 0;
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &iid, 0, &iidL);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt, "execute context fail");

    SQLINTEGER b = 0, t = 0;
    SQLBindCol(stmt, 1, SQL_C_LONG, &b, 0, NULL);
    SQLBindCol(stmt, 2, SQL_C_LONG, &t, 0, NULL);

    if (SQLFetch(stmt) == SQL_SUCCESS) {
        billId = (int)b;
        tableId = (int)t;
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return true;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return false;
}
bool BillDAL::listItemsByBill(int billId, vector<BillItem> &out) {
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc fail");
    const char *q = "SELECT bill_item_id, bill_id, food_id, quantity, description, sub_total FROM bill_items WHERE bill_id=? ORDER BY bill_item_id";
    if (SQLPrepare(stmt, (SQLCHAR *)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare list items fail");
    SQLINTEGER id = billId;
    SQLLEN idL = 0;
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, &idL);
    if (SQLExecute(stmt) != SQL_SUCCESS && SQLExecute(stmt) != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt, "exec list items fail");
    SQLINTEGER bi, b, f, qty;
    SQLDOUBLE sub;
    SQLCHAR desc[260];
    SQLLEN dLen;
    SQLBindCol(stmt, 1, SQL_C_LONG, &bi, 0, NULL);
    SQLBindCol(stmt, 2, SQL_C_LONG, &b, 0, NULL);
    SQLBindCol(stmt, 3, SQL_C_LONG, &f, 0, NULL);
    SQLBindCol(stmt, 4, SQL_C_LONG, &qty, 0, NULL);
    SQLBindCol(stmt, 5, SQL_C_CHAR, desc, sizeof(desc), &dLen);
    SQLBindCol(stmt, 6, SQL_C_DOUBLE, &sub, 0, NULL);
    while (SQLFetch(stmt) == SQL_SUCCESS) {
        BillItem it;
        it.id = bi;
        it.bill_id = b;
        it.food_id = f;
        it.quantity = qty;
        it.description = (dLen == SQL_NULL_DATA) ? "" : string((char *)desc);
        it.sub_total = sub;
        out.push_back(it);
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return true;
}

bool BillDAL::closeBill(int billId) {
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc fail");

    const char *q = "UPDATE bills SET paid_date = GETDATE() WHERE bill_id=? AND paid_date IS NULL";
    if (SQLPrepare(stmt, (SQLCHAR *)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare close fail");

    SQLINTEGER id = billId;
    SQLLEN idL = 0;
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, &idL);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt, "exec close fail");
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return true;
}

int BillDAL::countItems(int billId) {
    SQLHANDLE conn = DatabaseConnection::instance().getConnection();
    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS)
        throw runtime_error("alloc stmt fail");

    const char *q = "SELECT COUNT(*) FROM bill_items WHERE bill_id=?";
    if (SQLPrepare(stmt, (SQLCHAR *)q, SQL_NTS) != SQL_SUCCESS)
        throwStmtErr(stmt, "prepare count items fail");

    SQLINTEGER id = billId;
    SQLLEN idL = 0;
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, &idL);

    SQLRETURN rc = SQLExecute(stmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        throwStmtErr(stmt, "execute count items fail");

    SQLINTEGER c = 0;
    SQLBindCol(stmt, 1, SQL_C_LONG, &c, 0, NULL);
    int result = 0;
    if (SQLFetch(stmt) == SQL_SUCCESS)
        result = (int)c;
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return result;
}

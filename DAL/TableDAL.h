#ifndef TABLE_DAL_H
#define TABLE_DAL_H
#include "DatabaseConnection.h"
#include "../Models/Table.h"
#include <vector>
using namespace std;

class TableDAL {
	public:
		vector<Table> getAll();
		bool getById(int id, Table& out);
		bool insert(const Table& table);
		bool update(const Table& table);
		bool remove(int id);
};
#endif

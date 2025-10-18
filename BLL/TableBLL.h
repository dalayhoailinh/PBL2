#ifndef TABLE_BLL_H
#define TABLE_BLL_H
#include "../DAL/TableDAL.h"
#include "../Models/Table.h"
#include "Result.h"
#include <vector>
using namespace std;

class TableBLL {
	private:
		TableDAL& tableDal;

		Result validateNumber(int number, int excludeId = -1);
		Result validateCapacity(int capacity);
		Result validateStatus(int status_id);

	public:
		TableBLL(TableDAL& tDal) : tableDal(tDal) {};
		vector<Table> getAll();
		bool getById(int id, Table& out);

		Result addChecked(const Table& table);
		Result updateChecked(const Table& table);
		Result removeChecked(int id);
};
#endif

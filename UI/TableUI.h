#ifndef TABLE_UI_H
#define TABLE_UI_H
#include "../BLL/TableBLL.h"
#include "../BLL/Result.h"
using namespace std;

class TableUI {
	private:
		TableBLL& tableBll;

		void printTableHeader();
		void printTableRow(const Table& t);

	public:
		TableUI(TableBLL& tBll) : tableBll(tBll) {};
		void menu();
		void listTables();
		void addTable();
		void editTable();
		void deleteTable();
};
#endif

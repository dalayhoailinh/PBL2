#ifndef BILL_BLL_H
#define BILL_BLL_H

#include "../DAL/BillDAL.h"
#include "../DAL/TableDAL.h"
#include "Result.h"
#include <string>
#include <vector>
using namespace std;

class BillBLL {
	private:
	    BillDAL &billDal;
	    TableDAL &tableDal;
	    
	    Result validateFoodId(int foodId);
	    Result validateQuantity(int quantity);
	    Result validateDesc(const string& desc);
	
	public:
	    BillBLL(BillDAL &bDal, TableDAL &tDal) : billDal(bDal), tableDal(tDal) {}
	    
	    bool getOpenBillByTableId(int tableId, Bill &out);
	    bool getCurrentItemsOfTable(int tableId, vector<BillItem> &outItems,
	                                Bill *outBill = NULL, double *outTotal = NULL);
	    Result validateTable(int tableId);
	    Result checkoutTableChecked(int tableId);
	    Result addFoodToTableChecked(int tableId, int foodId, int quantity, const string &desc);
	    Result updateBillItemChecked(int billItemId, int quantity, const string &desc);
	    Result deleteBillItemChecked(int billItemId);
	    bool checkoutTable(int tableId);
	    bool addFoodToTable(int tableId, int foodId, int quantity, const string &desc);
	    bool updateBillItem(int billItemId, int quantity, const string &desc);
	    bool deleteBillItem(int billItemId);
};
#endif


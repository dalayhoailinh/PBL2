#include"TableBLL.h"
using namespace std;

vector<Table> TableBLL::getAll() {
	return tableDal.getAll();
}

bool TableBLL::getById(int id, Table& out) {
	return tableDal.getById(id, out);
}

Result TableBLL::validateNumber(int number, int excludeId) {
	Result r;
	r.ok = false;
	r.message = "So ban khong hop le.";
	if(number <= 0) {
		r.message = "So ban phai lon hon 0";
		return r;
	}
	vector<Table> table = tableDal.getAll();
	for (size_t i = 0; i < table.size(); ++i) {
		if(table[i].number == number && table[i].id != excludeId) {
			r.message = "So ban da ton tai";
			return r;
		}
	}
	r.ok = true;
	r.message = "";
	return r;
}

Result TableBLL::validateCapacity(int capacity) {
	Result r;
	r.ok = false;
	r.message = "So cho ngoi khong hop le";
	if(capacity <= 0) {
		r.message = "So cho ngoi phai lon hon 0";
		return r;
	}
	if(capacity>20) {
		r.message = "So cho ngoi qua lon (=<20 cho ngoi)";
		return r;
	}
	r.ok = true;
	r.message = "";
	return r;
}

Result TableBLL::validateStatus(int status_id) {
	Result r;
	r.ok = false;
	r.message = "Trang thai khong hop le";
	if(status_id != 0 && status_id != 1) {
		r.message = "Status chi duoc phep la 0 (trong) hoac 1 (co khach)";
		return r;
	}
	r.ok = true;
	r.message = "";
	return r;
}

Result TableBLL::addChecked(const Table& table) {
	Result r;

	r = validateNumber(table.number); // excludeId = -1 (default)
	if(!r.ok) return r;
	r = validateCapacity(table.capacity);
	if(!r.ok) return r;
	r = validateStatus(table.status_id);
	if(!r.ok) return r;

	bool ok = tableDal.insert(table);
	r.ok = ok;
	r.message = ok ? "Them thanh cong" : "Them that bai";
	return r;
}

Result TableBLL::updateChecked(const Table& table) {
	Result r;
	
	Table exists;
	if(!tableDal.getById(table.id, exists)) {
		r.ok = false;
		r.message = "Ban khong ton tai";
		return r;
	}
	
	r = validateNumber(table.number, table.id); // excludeId = table.id
	if(!r.ok) return r;
	r = validateCapacity(table.capacity);
	if(!r.ok) return r;
	r = validateStatus(table.status_id);
	if(!r.ok) return r;
	
	bool ok = tableDal.update(table);
	r.ok = ok;
	r.message = ok ? "Sua ban thanh cong" : "Sua ban that bai";
	return r;
}

Result TableBLL::removeChecked(int id) {
	Result r;

	Table exists;
	if(!tableDal.getById(id, exists)) {
		r.ok = false;
		r.message = "Ban khong ton tai";
		return r;
	}

	bool ok = tableDal.remove(id);
	r.ok = ok;
	r.message = ok ? "Xoa ban thanh cong" : "Xoa ban that bai";
	return r;
}

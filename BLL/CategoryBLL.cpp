#include "CategoryBLL.h"
using namespace std;

vector<Category> CategoryBLL::getAll() {
	return categoryDal.getAll();
}

bool CategoryBLL::getById(int id, Category& out) {
	return categoryDal.getById(id, out);
}

Result CategoryBLL::validateName(const string& name) {
	Result r;
	r.ok=false;
	r.message="Ten danh muc khong hop le";
	if (name.size()==0) {
		r.message="Ten danh muc khong duoc rong";
		return r;
	}
	if (name.size()>100) {
		r.message="Ten danh muc qua dai (<=100 ky tu)";
		return r;
	}
	r.ok=true;
	r.message="";
	return r;
}

Result CategoryBLL::addChecked(const Category& category) {
	Result r;

	r = validateName(category.name);
	if(!r.ok) return r;

	bool ok = categoryDal.insert(category);
	r.ok = ok;
	r.message = ok ? "Them thanh cong" : "Them that bai";
	return r;
}

Result CategoryBLL::updateChecked(const Category& category) {
	Result r;

	Category exists;
	if (!categoryDal.getById(category.id, exists)) {
		r.ok=false;
		r.message="Danh muc khong ton tai";
		return r;
	}

	r = validateName(category.name);
	if(!r.ok) return r;

	bool ok = categoryDal.update(category);
	r.ok = ok;
	r.message = ok ? "Sua thanh cong" : "Sua that bai";
	return r;
}

Result CategoryBLL::removeChecked(int id) {
	Result r;
	Category exists;
	if (!categoryDal.getById(id, exists)) {
		r.ok=false;
		r.message="Danh muc khong ton tai";
		return r;
	}
	bool ok = categoryDal.remove(id);
	r.ok = ok;
	r.message = ok ? "Xoa thanh cong" : "Xoa that bai";
	return r;
}

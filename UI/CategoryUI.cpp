#include "CategoryUI.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
using namespace std;

void CategoryUI::printCategoryHeader() {
	cout << string(45, '=') << endl;
	cout << left << setw(6) << "ID"
	     << setw(12) << "Ten danh muc"
	     << endl;
	cout << string(45, '-') << endl;
}

void CategoryUI::printCategoryRow(const Category& c) {
	cout << left << setw(6) << c.id
	     << setw(12) << c.name << endl;
}

void CategoryUI::listCategories() {
	try {
		vector<Category> rows = categoryBll.getAll();
		cout << "\n=== DANH SACH DANH MUC ===\n";
		if (rows.empty()) {
			cout << "Khong co danh muc nao trong he thong.\n";
			return;
		}
		printCategoryHeader();
		for (size_t i=0; i<rows.size(); ++i) printCategoryRow(rows[i]);
		cout << string(45, '=') << endl;
	} catch (const exception& e) {
		cout << "Loi khi lay danh sach ban: " << e.what() << endl;
	}
}

void CategoryUI::addCategory() {
	cout << "\n=== THEM DANH MUC MOI ===\n";
	Category c;
	c.id=0;
	c.name="";

	cout << "Nhap ten danh muc: ";
	getline(cin, c.name);

	Result r = categoryBll.addChecked(c);
	cout << r.message << "\n";
}

void CategoryUI::editCategory() {
	int id;
	cout << "\n=== SUA THONG TIN DANH MUC ===\n";
	cout << "Nhap category_id can sua: ";
	cin >> id;
	cin.ignore();

	Category c;
	if (!categoryBll.getById(id, c)) {
		cout << "Khong tim thay danh muc co ID = " << id << "\n";
		return;
	}

	cout << "\nThong tin danh muc hien tai:\n";
	printCategoryHeader();
	printCategoryRow(c);
	cout << string(45, '-') << endl;

	cout << "Nhap ten danh muc moi (" << c.name << "): ";
	string name;
	getline(cin, name);
	if (!name.empty()) c.name = name;

	Result r = categoryBll.updateChecked(c);
	cout << r.message << "\n";
}

void CategoryUI::deleteCategory() {
	int id;
	cout << "\n=== XOA DANH MUC ===\n";
	cout << "Nhap category_id can xoa: ";
	cin >> id;
	cin.ignore();

	Category c;
	if (!categoryBll.getById(id, c)) {
		cout << "Khong tim thay ban co ID = " << id << "\n";
		return;
	}

	cout << "\nThong tin danh muc se bi xoa:\n";
	printCategoryHeader();
	printCategoryRow(c);
	cout << string(45, '-') << endl;

	cout << "Ban co chac chan muon xoa danh muc nay? (y/n): ";
	char confirm;
	cin >> confirm;
	cin.ignore();

	if (confirm == 'y' || confirm == 'Y') {
		Result r = categoryBll.removeChecked(id);
		cout << r.message << "\n";
	} else {
		cout << "Da huy thao tac xoa danh muc.\n";
	}
}

void CategoryUI::menu() {
	while (true) {
		cout << "\n=== Quan ly danh muc===\n";
		cout << "1. Xem danh sach danh muc\n";
		cout << "2. Them danh muc\n";
		cout << "3. Sua danh muc\n";
		cout << "4. Xoa danh muc\n";
		cout << "0. Quay lai\n";
		cout << "Chon: ";

		int c;
		if (!(cin >> c)) return;
		cin.ignore();

		if (c==1) listCategories();
		else if (c==2) addCategory();
		else if (c==3) editCategory();
		else if (c==4) deleteCategory();
		else if (c==0) break;
		else cout << "Lua chon khong hop le\n";
	}
}

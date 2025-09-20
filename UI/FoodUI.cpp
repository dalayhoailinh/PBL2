#include "FoodUI.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
using namespace std;

void FoodUI::printFoodHeader(){
    cout << left << setw(6) << "ID"
         << setw(28) << "Ten mon"
         << setw(22) << "Ten danh muc"
         << "Gia\n";
}

void FoodUI::printFoodRow(const Food& f){
    cout << left << setw(6) << f.id
         << setw(28) << f.name
         << setw(22) << f.categoryName
         << fixed << setprecision(2) << f.price << "\n";
}

void FoodUI::listFoods(){
    vector<Food> rows = foodBll.getAll();
    cout << "=== Danh sach mon an ===\n";
    printFoodHeader();
    for (size_t i=0;i<rows.size();++i) printFoodRow(rows[i]);
}

void FoodUI::addFood(){
    Food f; f.id=0; f.name=""; f.categoryId=0; f.categoryName=""; f.price=0.0;

    cout << "Nhap ten mon: ";
    getline(cin, f.name);

    cout << "Chon danh muc:\n";
    vector<Category> cats = categoryBll.getAll();
    for (size_t i=0;i<cats.size();++i)
        cout << "  " << cats[i].id << " - " << cats[i].name << "\n";

    cout << "Nhap id danh muc: ";
    cin >> f.categoryId;
    cout << "Nhap gia: ";
    cin >> f.price;
    cin.ignore();

    Result r = foodBll.addChecked(f);
    cout << r.message << "\n";
}

void FoodUI::editFood(){
    int id;
    cout << "Nhap food_id can sua: ";
    cin >> id;
    cin.ignore();

    Food f;
    if (!foodBll.getById(id, f)){ cout << "Khong tim thay\n"; return; }

    cout << "Nhap ten mon moi (de trong neu giu nguyen): ";
    string name; getline(cin, name);
    if (!name.empty()) f.name = name;

    cout << "Danh muc hien tai: " << f.categoryName << "\n";
    cout << "Nhap category_id moi (0 de giu): ";
    int cid; cin >> cid;
    if (cid != 0) f.categoryId = cid;

    cout << "Gia hien tai: " << fixed << setprecision(2) << f.price << "\n";
    cout << "Nhap gia moi (am de giu): ";
    double p; cin >> p; cin.ignore();
    if (p >= 0) f.price = p;

    Result r = foodBll.updateChecked(f);
    cout << r.message << "\n";
}

void FoodUI::deleteFood(){
    int id;
    cout << "Nhap food_id can xoa: ";
    cin >> id;
    cin.ignore();

    Result r = foodBll.removeChecked(id);
    cout << r.message << "\n";
}

void FoodUI::menu(){
    while (true){
        cout << "\n=== Quan ly mon an (khong dau) ===\n";
        cout << "1. Xem danh sach mon\n";
        cout << "2. Them mon\n";
        cout << "3. Sua mon\n";
        cout << "4. Xoa mon\n";
        cout << "0. Quay lai\n";
        cout << "Chon: ";

        int c;
        if (!(cin >> c)) return;
        cin.ignore();

        if (c==1) listFoods();
        else if (c==2) addFood();
        else if (c==3) editFood();
        else if (c==4) deleteFood();
        else if (c==0) break;
        else cout << "Lua chon khong hop le\n";
    }
}


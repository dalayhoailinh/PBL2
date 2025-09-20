#include "FoodBLL.h"
using namespace std;

vector<Food> FoodBLL::getAll(){
    return foodDal.getAll();
}

bool FoodBLL::getById(int id, Food& out){
    return foodDal.getById(id, out);
}

Result FoodBLL::validateName(const string& name){
    Result r; r.ok=false; r.message="Ten mon khong hop le";
    if (name.size()==0){ r.message="Ten mon khong duoc rong"; return r; }
    if (name.size()>100){ r.message="Ten mon qua dai (<=100 ky tu)"; return r; }
    r.ok=true; r.message=""; return r;
}

Result FoodBLL::validateCategory(int categoryId){
    Result r; r.ok=false; r.message="Danh muc khong hop le";
    if (categoryId<=0){ r.message="CategoryId phai > 0"; return r; }
    Category c;
    if (!categoryDal.getById(categoryId, c)){ r.message="Danh muc khong ton tai"; return r; }
    r.ok=true; r.message=""; return r;
}

Result FoodBLL::validatePrice(double price){
    Result r; r.ok=false; r.message="Gia khong hop le";
    if (price<0){ r.message="Gia phai >= 0"; return r; }
    if (price>100000000){ r.message="Gia qua lon"; return r; }
    r.ok=true; r.message=""; return r;
}

Result FoodBLL::addChecked(const Food& food){
    Result r;

    r = validateName(food.name); if(!r.ok) return r;
    r = validateCategory(food.categoryId); if(!r.ok) return r;
    r = validatePrice(food.price); if(!r.ok) return r;

    bool ok = foodDal.insert(food);
    r.ok = ok;
    r.message = ok ? "Them thanh cong" : "Them that bai";
    return r;
}

Result FoodBLL::updateChecked(const Food& food){
    Result r;

    Food exists;
    if (!foodDal.getById(food.id, exists)){ r.ok=false; r.message="Mon khong ton tai"; return r; }

    r = validateName(food.name); if(!r.ok) return r;
    r = validateCategory(food.categoryId); if(!r.ok) return r;
    r = validatePrice(food.price); if(!r.ok) return r;

    bool ok = foodDal.update(food);
    r.ok = ok;
    r.message = ok ? "Sua thanh cong" : "Sua that bai";
    return r;
}

Result FoodBLL::removeChecked(int id){
    Result r;
    Food exists;
    if (!foodDal.getById(id, exists)){ r.ok=false; r.message="Mon khong ton tai"; return r; }
    bool ok = foodDal.remove(id);
    r.ok = ok;
    r.message = ok ? "Xoa thanh cong" : "Xoa that bai";
    return r;
}


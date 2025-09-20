#include "CategoryBLL.h"
using namespace std;

vector<Category> CategoryBLL::getAll(){
    return categoryDal.getAll();
}

bool CategoryBLL::getById(int id, Category& out){
    return categoryDal.getById(id, out);
}


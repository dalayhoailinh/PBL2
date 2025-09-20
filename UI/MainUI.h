#ifndef MAIN_UI_H
#define MAIN_UI_H
#include "FoodUI.h"
using namespace std;

class MainUI {
private:
    FoodUI& foodUI;
public:
	MainUI(FoodUI& fUi) : foodUI(fUi){};
    void menu();
};
#endif


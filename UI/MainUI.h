#ifndef MAIN_UI_H
#define MAIN_UI_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#endif

#include "../BLL/CategoryBLL.h"
#include "../BLL/FoodBLL.h"

#include "AdminUI.h"
#include "CategoryUI.h"
#include "FoodUI.h"

class MainUI {
    public:
        MainUI(FoodBLL &foodBll, CategoryBLL &categoryBll);

        void run(); // vòng lặp chính

    private:
        // ----- Trạng thái tổng -----
        enum FocusPane { F_LEFT = 0,
                         F_RIGHT = 1 };
        enum Key { K_NONE = 0,
                   K_ESC,
                   K_ENTER,
                   K_UP,
                   K_DOWN,
                   K_LEFT,
                   K_RIGHT,
                   K_BACK };

        FoodBLL &_foodBll;
        CategoryBLL &_categoryBll;

        AdminUI _adminUI;
        FoodUI _foodUI;
        CategoryUI _categoryUI;

        FocusPane _focus;
        bool _running;

        int _prevRows, _prevCols; // phát hiện resize
        int _lastModule;          // 0=Food, 1=Category
        bool _firstFrame;

        // ----- Console helpers / ANSI -----
        static void enableVT();
        static void clear();
        static void home();
        static void reset();
        static void bold();
        static void gray();
        static void rev();
        static void hideCursor();
        static void showCursor();
        static void gotoRC(int r, int c);

        static void drawH(int row, int c1, int c2, char ch);
        static void drawV(int col, int r1, int r2, char ch);
        static void drawBox(int r1, int c1, int r2, int c2);
        static void padRight(const string &s, int w);

        static void termSize(int &rows, int &cols);
        static Key readKey();

        // ----- Layout tổng -----
        void renderFrame(int rows, int cols,
                         int &Lr1, int &Lc1, int &Lr2, int &Lc2,
                         int &Rr1, int &Rc1, int &Rr2, int &Rc2);

        void renderRightTitle(int right_r1, int right_c1, const string &title);

        // ----- Điều phối trái/phải -----
        void handleLeftKey(Key k, bool &moduleChanged);
        void handleRightKey(Key k);

        // ----- Resize handling -----
        void onResized(); // gọi khi kích thước thay đổi
};

#endif // MAIN_UI_H

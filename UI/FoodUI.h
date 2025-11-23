#ifndef FOOD_UI_H
#define FOOD_UI_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "../BLL/FoodBLL.h"

// UI phải cho Món ăn: MENU -> LIST (bảng) -> DETAIL
// Tối ưu: renderFull/renderDelta, onResize, resetView, cache dòng bảng (first, prevSel,...)
class FoodUI {
    public:
        explicit FoodUI(FoodBLL &foodBll);

        string title() const;

        void renderFull(int r1, int c1, int r2, int c2, bool active);
        void renderDelta(int r1, int c1, int r2, int c2, bool active);

        void onUp();
        void onDown();
        void onEnter();
        void onBack();

        void onResize();  // reset cache khi cửa sổ đổi kích thước
        void resetView(); // khi đổi module

    private:
        FoodBLL &_foodBll;

        enum View { V_MENU = 0,
                    V_LIST = 1,
                    V_DETAIL = 2 };
        View _view;

        // MENU state
        int _menuSel; // 0..3
        int _menuPrev;

        // LIST state
        int _listSel;
        int _listPrev;
        int _first; // scroll window
        int _firstPrev;

        bool _lastActive;
        bool _invalidFull; // cần full redraw (khi view đổi / scroll đổi / resize)

        // helpers ANSI
        static void gotoRC(int r, int c);
        static void bold();
        static void reset();
        static void rev();
        static void gray();
        static void padRight(const string &s, int w);
        static void padLeft(const string &s, int w);
        static string toIntStr(int v);
        static string toPriceStr(double p);

        // render theo view
        void renderMenuFull(int r1, int c1, int r2, int c2, bool active);
        void renderMenuDelta(int r1, int c1, int r2, int c2, bool active);

        void renderTableFull(int r1, int c1, int r2, int c2, bool active);
        void renderTableDelta(int r1, int c1, int r2, int c2, bool active);

        void renderDetailFull(int r1, int c1, int r2, int c2);

        // layout bảng
        void computeFoodCols(int content_w, int &wID, int &wName, int &wCat, int &wPrice);
};

#endif // FOOD_UI_H

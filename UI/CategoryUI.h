#ifndef CATEGORY_UI_H
#define CATEGORY_UI_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "../BLL/CategoryBLL.h"

// UI phải cho Danh mục: MENU -> LIST (bảng ID|Ten) -> DETAIL
class CategoryUI {
    public:
        explicit CategoryUI(CategoryBLL &categoryBll);

        string title() const;

        void renderFull(int r1, int c1, int r2, int c2, bool active);
        void renderDelta(int r1, int c1, int r2, int c2, bool active);

        void onUp();
        void onDown();
        void onEnter();
        void onBack();

        void onResize();
        void resetView();

    private:
        CategoryBLL &_categoryBll;

        enum View { V_MENU = 0,
                    V_LIST = 1,
                    V_DETAIL = 2 };
        View _view;

        int _menuSel, _menuPrev;
        int _listSel, _listPrev;
        int _first, _firstPrev;

        bool _lastActive;
        bool _invalidFull;

        // helpers
        static void gotoRC(int r, int c);
        static void bold();
        static void reset();
        static void rev();
        static void gray();
        static void padRight(const string &s, int w);
        static void padLeft(const string &s, int w);
        static string toIntStr(int v);

        void computeCatCols(int content_w, int &wID, int &wName);

        // render by view
        void renderMenuFull(int r1, int c1, int r2, int c2, bool active);
        void renderMenuDelta(int r1, int c1, int r2, int c2, bool active);

        void renderTableFull(int r1, int c1, int r2, int c2, bool active);
        void renderTableDelta(int r1, int c1, int r2, int c2, bool active);

        void renderDetailFull(int r1, int c1, int r2, int c2);
};

#endif // CATEGORY_UI_H

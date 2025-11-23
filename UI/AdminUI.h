#ifndef ADMIN_UI_H
#define ADMIN_UI_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

// Cột trái: Admin menu (Quản lý món ăn | Quản lý danh mục)
// Thiết kế lại: có renderFull/renderDelta, onResize, lưu _lastActive & _selPrev để vẽ delta.
class AdminUI {
    public:
        AdminUI();

        void renderFull(int r1, int c1, int r2, int c2, bool active);
        void renderDelta(int r1, int c1, int r2, int c2, bool active);

        void moveUp();
        void moveDown();

        int selectedIndex() const;

        void onResize(); // nếu sau này có cache, clear ở đây

    private:
        int _sel;
        int _selPrev;
        bool _lastActive;

        // helpers
        static void gotoRC(int r, int c);
        static void bold();
        static void reset();
        static void rev();
        static void padRight(const string &s, int w);

        void drawRow(int r, int c, int w, const string &text, bool selected, bool active);
};

#endif // ADMIN_UI_H

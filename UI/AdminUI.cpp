#include "AdminUI.h"

AdminUI::AdminUI() : _sel(0), _selPrev(0), _lastActive(false) {}

void AdminUI::gotoRC(int r, int c) { cout << "\x1b[" << r << ";" << c << "H"; }
void AdminUI::bold() { cout << "\x1b[1m"; }
void AdminUI::reset() { cout << "\x1b[0m"; }
void AdminUI::rev() { cout << "\x1b[7m"; }
void AdminUI::padRight(const string &s, int w) {
    if ((int)s.size() >= w)
        cout.write(s.c_str(), w);
    else {
        cout << s;
        for (int i = (int)s.size(); i < w; ++i)
            cout << ' ';
    }
}

void AdminUI::drawRow(int r, int c, int w, const string &text, bool selected, bool active) {
    gotoRC(r, c);
    if (selected && active) {
        rev();
        padRight("> " + text, w);
        reset();
    } else {
        padRight("  " + text, w);
    }
}

void AdminUI::renderFull(int r1, int c1, int r2, int c2, bool active) {
    vector<string> items;
    items.push_back("Quan ly mon an");
    items.push_back("Quan ly danh muc");
    int inner_w = (c2 - c1 - 3);
    int area_h = (r2 - r1 - 1);
    for (int i = 0; i < area_h; ++i) {
        string line = (i < (int)items.size()) ? items[i] : "";
        drawRow(r1 + 1 + i, c1 + 2, inner_w, line, i == _sel, active);
    }
    _selPrev = _sel;
    _lastActive = active;
}

void AdminUI::renderDelta(int r1, int c1, int r2, int c2, bool active) {
    vector<string> items;
    items.push_back("Quan ly mon an");
    items.push_back("Quan ly danh muc");
    int inner_w = (c2 - c1 - 3);
    if (_selPrev != _sel) {
        // vẽ lại dòng cũ (bỏ highlight)
        if (_selPrev >= 0 && _selPrev < (int)items.size())
            drawRow(r1 + 1 + _selPrev, c1 + 2, inner_w, items[_selPrev], false, active);
        // vẽ lại dòng mới (bật highlight nếu active)
        if (_sel >= 0 && _sel < (int)items.size())
            drawRow(r1 + 1 + _sel, c1 + 2, inner_w, items[_sel], true, active);
        _selPrev = _sel;
    } else if (_lastActive != active) {
        // chỉ đổi trạng thái focus → vẽ lại dòng đang chọn
        if (_sel >= 0 && _sel < (int)items.size())
            drawRow(r1 + 1 + _sel, c1 + 2, inner_w, items[_sel], true, active);
    }
    _lastActive = active;
}

void AdminUI::moveUp() { _sel = (_sel - 1 + 2) % 2; }
void AdminUI::moveDown() { _sel = (_sel + 1) % 2; }
int AdminUI::selectedIndex() const { return _sel; }
void AdminUI::onResize() { /* hiện chưa cần cache; để sẵn */ }

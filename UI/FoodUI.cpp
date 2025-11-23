#include "FoodUI.h"
#include <cstdio>

FoodUI::FoodUI(FoodBLL &foodBll)
    : _foodBll(foodBll),
      _view(V_MENU),
      _menuSel(0), _menuPrev(0),
      _listSel(0), _listPrev(0),
      _first(0), _firstPrev(0),
      _lastActive(false),
      _invalidFull(true) {}

void FoodUI::gotoRC(int r, int c) { cout << "\x1b[" << r << ";" << c << "H"; }
void FoodUI::bold() { cout << "\x1b[1m"; }
void FoodUI::reset() { cout << "\x1b[0m"; }
void FoodUI::rev() { cout << "\x1b[7m"; }
void FoodUI::gray() { cout << "\x1b[90m"; }
void FoodUI::padRight(const string &s, int w) {
    if ((int)s.size() >= w)
        cout.write(s.c_str(), w);
    else {
        cout << s;
        for (int i = (int)s.size(); i < w; ++i)
            cout << ' ';
    }
}
void FoodUI::padLeft(const string &s, int w) {
    int n = (int)s.size();
    if (n >= w)
        cout.write(s.c_str(), w);
    else {
        for (int i = 0; i < w - n; ++i)
            cout << ' ';
        cout << s;
    }
}
string FoodUI::toIntStr(int v) {
    char b[32];
    sprintf(b, "%d", v);
    return string(b);
}
string FoodUI::toPriceStr(double p) {
    char b[64];
    sprintf(b, "%.0f", p);
    return string(b);
}

string FoodUI::title() const {
    if (_view == V_MENU)
        return "LUA CHON - MON AN";
    if (_view == V_LIST)
        return "DANH SACH MON AN";
    return "CHI TIET MON AN";
}

void FoodUI::computeFoodCols(int content_w, int &wID, int &wName, int &wCat, int &wPrice) {
    int sep = 9;
    int minID = 4, minPrice = 8, minName = 12, minCat = 10;
    int fixed = minID + minPrice + sep;
    int rem = content_w - fixed;
    if (rem < minName + minCat) {
        int half = (rem > 0) ? rem / 2 : 1;
        wName = (half < 1) ? 1 : half;
        wCat = rem - wName;
        if (wCat < 1)
            wCat = 1;
    } else {
        wName = minName + (int)((rem - (minName + minCat)) * 0.6) + minCat;
        wCat = content_w - (sep + minID + minPrice + wName);
        if (wCat < minCat) {
            wCat = minCat;
            wName = content_w - (sep + minID + minPrice + wCat);
        }
    }
    wID = minID;
    wPrice = minPrice;
}

// ----- PUBLIC render entry -----
void FoodUI::renderFull(int r1, int c1, int r2, int c2, bool active) {
    if (_view == V_MENU)
        renderMenuFull(r1 + 1, c1 + 2, r2, c2, active);
    else if (_view == V_LIST)
        renderTableFull(r1 + 1, c1 + 2, r2, c2, active);
    else
        renderDetailFull(r1 + 1, c1 + 2, r2, c2);
    _lastActive = active;
    _invalidFull = false;
}
void FoodUI::renderDelta(int r1, int c1, int r2, int c2, bool active) {
    if (_invalidFull) {
        renderFull(r1, c1, r2, c2, active);
        return;
    }

    if (_view == V_MENU)
        renderMenuDelta(r1 + 1, c1 + 2, r2, c2, active);
    else if (_view == V_LIST)
        renderTableDelta(r1 + 1, c1 + 2, r2, c2, active);
    else { /* DETAIL: giữ nguyên cho đến khi back/enter */
    }

    _lastActive = active;
}

// ----- MENU -----
void FoodUI::renderMenuFull(int r1, int c1, int r2, int c2, bool active) {
    vector<string> choices;
    choices.push_back("Xem danh sach mon an");
    choices.push_back("Them mon an");
    choices.push_back("Chinh sua mon an");
    choices.push_back("Xoa mon an");

    int w = (c2 - c1 - 1);
    int h = (r2 - r1 - 0);

    for (int i = 0; i < h; ++i) {
        gotoRC(r1 + i, c1);
        string line = (i < (int)choices.size()) ? choices[i] : "";
        if (i == _menuSel && active) {
            rev();
            padRight("> " + line, w);
            reset();
        } else
            padRight("  " + line, w);
    }
    gotoRC(r2, c1);
    gray();
    cout << "Chon \"Xem danh sach\" de hien BANG (Enter)";
    reset();

    _menuPrev = _menuSel;
}
void FoodUI::renderMenuDelta(int r1, int c1, int r2, int c2, bool active) {
    vector<string> choices;
    choices.push_back("Xem danh sach mon an");
    choices.push_back("Them mon an");
    choices.push_back("Chinh sua mon an");
    choices.push_back("Xoa mon an");

    int w = (c2 - c1 - 1);
    if (_menuPrev != _menuSel) {
        // dòng cũ
        if (_menuPrev >= 0 && _menuPrev < (int)choices.size()) {
            gotoRC(r1 + _menuPrev, c1);
            padRight("  " + choices[_menuPrev], w);
        }
        // dòng mới
        if (_menuSel >= 0 && _menuSel < (int)choices.size()) {
            gotoRC(r1 + _menuSel, c1);
            if (active) {
                rev();
                padRight("> " + choices[_menuSel], w);
                reset();
            } else
                padRight("  " + choices[_menuSel], w);
        }
        _menuPrev = _menuSel;
    } else if (_lastActive != active) {
        // đổi focus → vẽ lại dòng đang chọn
        if (_menuSel >= 0 && _menuSel < (int)choices.size()) {
            gotoRC(r1 + _menuSel, c1);
            if (active) {
                rev();
                padRight("> " + choices[_menuSel], w);
                reset();
            } else
                padRight("  " + choices[_menuSel], w);
        }
    }
}

// ----- LIST (TABLE) -----
void FoodUI::renderTableFull(int r1, int c1, int r2, int c2, bool active) {
    const vector<Food> &foods = _foodBll.getAll();

    int cw = (c2 - c1 - 1), ch = (r2 - r1 - 0);
    int wID, wName, wCat, wPrice;
    computeFoodCols(cw, wID, wName, wCat, wPrice);

    // header
    gotoRC(r1, c1);
    bold();
    padRight("ID", wID);
    cout << " | ";
    padRight("Ten mon", wName);
    cout << " | ";
    padRight("Danh muc", wCat);
    cout << " | ";
    padRight("Gia", wPrice);
    reset();

    // underline
    gotoRC(r1 + 1, c1);
    for (int i = 0; i < cw; ++i)
        cout << '-';

    int body_rows = ch - 2;
    if (body_rows < 1)
        body_rows = 1;
    int n = (int)foods.size();
    int first = _first;
    if (_listSel < first)
        first = _listSel;
    if (_listSel >= first + body_rows)
        first = _listSel - body_rows + 1;
    _first = first;
    _firstPrev = first;

    for (int i = 0; i < body_rows; ++i) {
        int idx = first + i;
        gotoRC(r1 + 2 + i, c1);
        if (idx < n) {
            const Food &f = foods[idx];
            bool hl = (idx == _listSel) && active;
            if (hl)
                rev();
            padLeft(toIntStr(f.id), wID);
            cout << " | ";
            padRight(f.name, wName);
            cout << " | ";
            padRight(f.categoryName, wCat);
            cout << " | ";
            padLeft(toPriceStr(f.price), wPrice);
            if (hl)
                reset();
            int printed = wID + 3 + wName + 3 + wCat + 3 + wPrice;
            for (int j = printed; j < cw; ++j)
                cout << ' ';
        } else {
            for (int j = 0; j < cw; ++j)
                cout << ' ';
        }
    }
    gotoRC(r2, c1);
    gray();
    cout << "Enter: chi tiet | Backspace: quay lai LUA CHON";
    reset();

    _listPrev = _listSel;
}
void FoodUI::renderTableDelta(int r1, int c1, int r2, int c2, bool active) {
    const vector<Food> &foods = _foodBll.getAll();

    int cw = (c2 - c1 - 1), ch = (r2 - r1 - 0);
    int body_rows = ch - 2;
    if (body_rows < 1)
        body_rows = 1;

    // tính lại window
    int first = _first;
    if (_listSel < first)
        first = _listSel;
    if (_listSel >= first + body_rows)
        first = _listSel - body_rows + 1;

    if (first != _firstPrev) {
        _first = first;
        _firstPrev = first;
        // scroll thay đổi → vẽ lại bảng đầy đủ
        renderTableFull(r1, c1, r2, c2, active);
        return;
    }

    // chỉ đổi chọn trong khung nhìn → vẽ 2 dòng: prev và curr
    int prev = _listPrev, curr = _listSel;
    if (prev == curr && _lastActive == active)
        return;

    // header/underline không đổi
    if (prev >= first && prev < first + body_rows) {
        int i = prev - first;
        gotoRC(r1 + 2 + i, c1);
        if (prev < (int)foods.size()) {
            const Food &f = foods[prev];
            bool hl = false; // dòng cũ bỏ highlight
            int wID, wName, wCat, wPrice;
            computeFoodCols(cw, wID, wName, wCat, wPrice);
            padLeft(toIntStr(f.id), wID);
            cout << " | ";
            padRight(f.name, wName);
            cout << " | ";
            padRight(f.categoryName, wCat);
            cout << " | ";
            padLeft(toPriceStr(f.price), wPrice);
            int printed = wID + 3 + wName + 3 + wCat + 3 + wPrice;
            for (int j = printed; j < cw; ++j)
                cout << ' ';
        } else {
            for (int j = 0; j < cw; ++j)
                cout << ' ';
        }
    }
    if (curr >= first && curr < first + body_rows) {
        int i = curr - first;
        gotoRC(r1 + 2 + i, c1);
        if (curr < (int)foods.size()) {
            const Food &f = foods[curr];
            bool hl = (active);
            if (hl)
                rev();
            int wID, wName, wCat, wPrice;
            computeFoodCols(cw, wID, wName, wCat, wPrice);
            padLeft(toIntStr(f.id), wID);
            cout << " | ";
            padRight(f.name, wName);
            cout << " | ";
            padRight(f.categoryName, wCat);
            cout << " | ";
            padLeft(toPriceStr(f.price), wPrice);
            if (hl)
                reset();
            int printed = wID + 3 + wName + 3 + wCat + 3 + wPrice;
            for (int j = printed; j < cw; ++j)
                cout << ' ';
        } else {
            for (int j = 0; j < cw; ++j)
                cout << ' ';
        }
    }
    _listPrev = _listSel;
}

// ----- DETAIL -----
void FoodUI::renderDetailFull(int r1, int c1, int r2, int c2) {
    const vector<Food> &foods = _foodBll.getAll();
    int w = (c2 - c1 - 1), h = (r2 - r1 - 0);

    vector<string> lines;
    if (!foods.empty()) {
        if (_listSel < 0)
            _listSel = 0;
        if (_listSel >= (int)foods.size())
            _listSel = (int)foods.size() - 1;
        const Food &f = foods[_listSel];
        char b1[64], b2[64];
        sprintf(b1, "ID: %d", f.id);
        sprintf(b2, "Gia: %.0f VND", f.price);
        lines.push_back("Chi tiet mon an");
        lines.push_back("----------------");
        lines.push_back(string(b1));
        lines.push_back(string("Ten: ") + f.name);
        lines.push_back(string("Danh muc: ") + f.categoryName);
        lines.push_back(string(b2));
    } else {
        lines.push_back("(Khong co du lieu)");
    }
    lines.push_back("");
    lines.push_back("Nhan phim bat ky de quay lai BANG...");

    for (int i = 0; i < h; ++i) {
        gotoRC(r1 + i, c1);
        if (i < (int)lines.size())
            padRight(lines[i], w);
        else
            padRight("", w);
    }
}

// ----- Inputs -----
void FoodUI::onUp() {
    if (_view == V_MENU) {
        _menuSel = (_menuSel - 1 + 4) % 4;
    } else if (_view == V_LIST) {
        int n = (int)_foodBll.getAll().size();
        if (n > 0)
            _listSel = (_listSel - 1 + n) % n;
    }
}
void FoodUI::onDown() {
    if (_view == V_MENU) {
        _menuSel = (_menuSel + 1) % 4;
    } else if (_view == V_LIST) {
        int n = (int)_foodBll.getAll().size();
        if (n > 0)
            _listSel = (_listSel + 1) % n;
    }
}
void FoodUI::onEnter() {
    if (_view == V_MENU) {
        if (_menuSel == 0) {
            _view = V_LIST;
            _listSel = 0;
            _first = 0;
            _invalidFull = true;
        } else { /* TODO: Them/Chinh sua/Xoa */
        }
    } else if (_view == V_LIST) {
        _view = V_DETAIL;
        _invalidFull = true;
    } else if (_view == V_DETAIL) {
        _view = V_LIST;
        _invalidFull = true;
    }
}
void FoodUI::onBack() {
    if (_view == V_LIST) {
        _view = V_MENU;
        _invalidFull = true;
    } else if (_view == V_DETAIL) {
        _view = V_LIST;
        _invalidFull = true;
    }
}
void FoodUI::onResize() {
    _first = 0;
    _firstPrev = 0;
    _invalidFull = true;
}
void FoodUI::resetView() {
    _view = V_MENU;
    _menuSel = 0;
    _menuPrev = 0;
    _listSel = 0;
    _listPrev = 0;
    _first = 0;
    _firstPrev = 0;
    _invalidFull = true;
}

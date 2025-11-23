#include "CategoryUI.h"
#include <cstdio>

CategoryUI::CategoryUI(CategoryBLL &categoryBll)
    : _categoryBll(categoryBll),
      _view(V_MENU),
      _menuSel(0), _menuPrev(0),
      _listSel(0), _listPrev(0),
      _first(0), _firstPrev(0),
      _lastActive(false),
      _invalidFull(true) {}

void CategoryUI::gotoRC(int r, int c) { cout << "\x1b[" << r << ";" << c << "H"; }
void CategoryUI::bold() { cout << "\x1b[1m"; }
void CategoryUI::reset() { cout << "\x1b[0m"; }
void CategoryUI::rev() { cout << "\x1b[7m"; }
void CategoryUI::gray() { cout << "\x1b[90m"; }
void CategoryUI::padRight(const string &s, int w) {
    if ((int)s.size() >= w)
        cout.write(s.c_str(), w);
    else {
        cout << s;
        for (int i = (int)s.size(); i < w; ++i)
            cout << ' ';
    }
}
void CategoryUI::padLeft(const string &s, int w) {
    int n = (int)s.size();
    if (n >= w)
        cout.write(s.c_str(), w);
    else {
        for (int i = 0; i < w - n; ++i)
            cout << ' ';
        cout << s;
    }
}
string CategoryUI::toIntStr(int v) {
    char b[32];
    sprintf(b, "%d", v);
    return string(b);
}

string CategoryUI::title() const {
    if (_view == V_MENU)
        return "LUA CHON - DANH MUC";
    if (_view == V_LIST)
        return "DANH SACH DANH MUC";
    return "CHI TIET DANH MUC";
}

void CategoryUI::computeCatCols(int content_w, int &wID, int &wName) {
    int sep = 3;
    int minID = 4;
    wID = minID;
    wName = content_w - sep - wID;
    if (wName < 1)
        wName = 1;
}

void CategoryUI::renderFull(int r1, int c1, int r2, int c2, bool active) {
    if (_view == V_MENU)
        renderMenuFull(r1 + 1, c1 + 2, r2, c2, active);
    else if (_view == V_LIST)
        renderTableFull(r1 + 1, c1 + 2, r2, c2, active);
    else
        renderDetailFull(r1 + 1, c1 + 2, r2, c2);
    _lastActive = active;
    _invalidFull = false;
}
void CategoryUI::renderDelta(int r1, int c1, int r2, int c2, bool active) {
    if (_invalidFull) {
        renderFull(r1, c1, r2, c2, active);
        return;
    }
    if (_view == V_MENU)
        renderMenuDelta(r1 + 1, c1 + 2, r2, c2, active);
    else if (_view == V_LIST)
        renderTableDelta(r1 + 1, c1 + 2, r2, c2, active);
    else { /* DETAIL giữ nguyên */
    }
    _lastActive = active;
}

// ----- MENU -----
void CategoryUI::renderMenuFull(int r1, int c1, int r2, int c2, bool active) {
    vector<string> choices;
    choices.push_back("Xem danh sach danh muc");
    choices.push_back("Them danh muc");
    choices.push_back("Chinh sua danh muc");
    choices.push_back("Xoa danh muc");

    int w = (c2 - c1 - 1), h = (r2 - r1 - 0);
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
void CategoryUI::renderMenuDelta(int r1, int c1, int r2, int c2, bool active) {
    vector<string> choices;
    choices.push_back("Xem danh sach danh muc");
    choices.push_back("Them danh muc");
    choices.push_back("Chinh sua danh muc");
    choices.push_back("Xoa danh muc");

    int w = (c2 - c1 - 1);
    if (_menuPrev != _menuSel) {
        if (_menuPrev >= 0 && _menuPrev < (int)choices.size()) {
            gotoRC(r1 + _menuPrev, c1);
            padRight("  " + choices[_menuPrev], w);
        }
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
void CategoryUI::renderTableFull(int r1, int c1, int r2, int c2, bool active) {
    const vector<Category> &cats = _categoryBll.getAll();

    int cw = (c2 - c1 - 1), ch = (r2 - r1 - 0);
    int wID, wName;
    computeCatCols(cw, wID, wName);

    gotoRC(r1, c1);
    bold();
    padRight("ID", wID);
    cout << " | ";
    padRight("Ten", wName);
    reset();
    gotoRC(r1 + 1, c1);
    for (int i = 0; i < cw; ++i)
        cout << '-';

    int body_rows = ch - 2;
    if (body_rows < 1)
        body_rows = 1;
    int n = (int)cats.size();
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
            const Category &c = cats[idx];
            bool hl = (idx == _listSel) && active;
            if (hl)
                rev();
            padLeft(toIntStr(c.id), wID);
            cout << " | ";
            padRight(c.name, wName);
            if (hl)
                reset();
            int printed = wID + 3 + wName;
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
void CategoryUI::renderTableDelta(int r1, int c1, int r2, int c2, bool active) {
    const vector<Category> &cats = _categoryBll.getAll();

    int cw = (c2 - c1 - 1), ch = (r2 - r1 - 0);
    int body_rows = ch - 2;
    if (body_rows < 1)
        body_rows = 1;

    int first = _first;
    if (_listSel < first)
        first = _listSel;
    if (_listSel >= first + body_rows)
        first = _listSel - body_rows + 1;

    if (first != _firstPrev) {
        _first = first;
        _firstPrev = first;
        renderTableFull(r1, c1, r2, c2, active);
        return;
    }

    int prev = _listPrev, curr = _listSel;
    if (prev == curr && _lastActive == active)
        return;

    int wID, wName;
    computeCatCols(cw, wID, wName);

    if (prev >= first && prev < first + body_rows) {
        int i = prev - first;
        gotoRC(r1 + 2 + i, c1);
        if (prev < (int)cats.size()) {
            const Category &c = cats[prev];
            padLeft(toIntStr(c.id), wID);
            cout << " | ";
            padRight(c.name, wName);
            int printed = wID + 3 + wName;
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
        if (curr < (int)cats.size()) {
            const Category &c = cats[curr];
            if (active)
                rev();
            padLeft(toIntStr(c.id), wID);
            cout << " | ";
            padRight(c.name, wName);
            if (active)
                reset();
            int printed = wID + 3 + wName;
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
void CategoryUI::renderDetailFull(int r1, int c1, int r2, int c2) {
    const vector<Category> &cats = _categoryBll.getAll();
    int w = (c2 - c1 - 1), h = (r2 - r1 - 0);

    vector<string> lines;
    if (!cats.empty()) {
        if (_listSel < 0)
            _listSel = 0;
        if (_listSel >= (int)cats.size())
            _listSel = (int)cats.size() - 1;
        const Category &k = cats[_listSel];
        char b1[64];
        sprintf(b1, "ID: %d", k.id);
        lines.push_back("Chi tiet danh muc");
        lines.push_back("-----------------");
        lines.push_back(string(b1));
        lines.push_back(string("Ten: ") + k.name);
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
void CategoryUI::onUp() {
    if (_view == V_MENU) {
        _menuSel = (_menuSel - 1 + 4) % 4;
    } else if (_view == V_LIST) {
        int n = (int)_categoryBll.getAll().size();
        if (n > 0)
            _listSel = (_listSel - 1 + n) % n;
    }
}
void CategoryUI::onDown() {
    if (_view == V_MENU) {
        _menuSel = (_menuSel + 1) % 4;
    } else if (_view == V_LIST) {
        int n = (int)_categoryBll.getAll().size();
        if (n > 0)
            _listSel = (_listSel + 1) % n;
    }
}
void CategoryUI::onEnter() {
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
void CategoryUI::onBack() {
    if (_view == V_LIST) {
        _view = V_MENU;
        _invalidFull = true;
    } else if (_view == V_DETAIL) {
        _view = V_LIST;
        _invalidFull = true;
    }
}
void CategoryUI::onResize() {
    _first = 0;
    _firstPrev = 0;
    _invalidFull = true;
}
void CategoryUI::resetView() {
    _view = V_MENU;
    _menuSel = 0;
    _menuPrev = 0;
    _listSel = 0;
    _listPrev = 0;
    _first = 0;
    _firstPrev = 0;
    _invalidFull = true;
}

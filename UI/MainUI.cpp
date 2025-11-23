#include "MainUI.h"
#include <cstdio>

#ifndef _WIN32
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#endif

MainUI::MainUI(FoodBLL &foodBll, CategoryBLL &categoryBll)
    : _foodBll(foodBll), _categoryBll(categoryBll), _adminUI(), _foodUI(foodBll), _categoryUI(categoryBll), _focus(F_LEFT), _running(true), _prevRows(-1), _prevCols(-1), _lastModule(0), _firstFrame(true) {}

void MainUI::enableVT() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD mode = 0;
        if (GetConsoleMode(hOut, &mode)) {
            mode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
            SetConsoleMode(hOut, mode);
        }
    }
#endif
}
void MainUI::clear() { cout << "\x1b[2J"; }
void MainUI::home() { cout << "\x1b[H"; }
void MainUI::reset() { cout << "\x1b[0m"; }
void MainUI::bold() { cout << "\x1b[1m"; }
void MainUI::gray() { cout << "\x1b[90m"; }
void MainUI::rev() { cout << "\x1b[7m"; }
void MainUI::hideCursor() { cout << "\x1b[?25l"; }
void MainUI::showCursor() { cout << "\x1b[?25h"; }
void MainUI::gotoRC(int r, int c) { cout << "\x1b[" << r << ";" << c << "H"; }

void MainUI::drawH(int row, int c1, int c2, char ch) {
    if (c2 < c1)
        return;
    gotoRC(row, c1);
    for (int c = c1; c <= c2; ++c)
        cout << ch;
}
void MainUI::drawV(int col, int r1, int r2, char ch) {
    if (r2 < r1)
        return;
    for (int r = r1; r <= r2; ++r) {
        gotoRC(r, col);
        cout << ch;
    }
}
void MainUI::drawBox(int r1, int c1, int r2, int c2) {
    if (r2 < r1 || c2 < c1)
        return;
    drawH(r1, c1, c2, '-');
    drawH(r2, c1, c2, '-');
    drawV(c1, r1, r2, '|');
    drawV(c2, r1, r2, '|');
    gotoRC(r1, c1);
    cout << '+';
    gotoRC(r1, c2);
    cout << '+';
    gotoRC(r2, c1);
    cout << '+';
    gotoRC(r2, c2);
    cout << '+';
}
void MainUI::padRight(const string &s, int w) {
    if ((int)s.size() >= w)
        cout.write(s.c_str(), w);
    else {
        cout << s;
        for (int i = (int)s.size(); i < w; ++i)
            cout << ' ';
    }
}
void MainUI::termSize(int &rows, int &cols) {
    rows = 25;
    cols = 100;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
#else
    winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        if (ws.ws_col > 0)
            cols = ws.ws_col;
        if (ws.ws_row > 0)
            rows = ws.ws_row;
    }
#endif
    if (cols < 60)
        cols = 60;
    if (rows < 20)
        rows = 20;
}

MainUI::Key MainUI::readKey() {
#ifdef _WIN32
    int ch = _getch();
    if (ch == 0 || ch == 224) {
        int k = _getch();
        if (k == 72)
            return K_UP;
        if (k == 80)
            return K_DOWN;
        if (k == 75)
            return K_LEFT;
        if (k == 77)
            return K_RIGHT;
        return K_NONE;
    }
    if (ch == '\r')
        return K_ENTER;
    if (ch == 27)
        return K_ESC;
    if (ch == 8)
        return K_BACK;
    return K_NONE;
#else
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios t = oldt;
    t.c_lflag &= ~(ICANON | ECHO);
    t.c_cc[VMIN] = 1;
    t.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
    unsigned char c = 0;
    if (read(STDIN_FILENO, &c, 1) != 1) {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return K_NONE;
    }
    if (c == 0x1B) {
        unsigned char b1 = 0, b2 = 0;
        read(STDIN_FILENO, &b1, 1);
        if (b1 == '[') {
            read(STDIN_FILENO, &b2, 1);
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            if (b2 == 'A')
                return K_UP;
            if (b2 == 'B')
                return K_DOWN;
            if (b2 == 'C')
                return K_RIGHT;
            if (b2 == 'D')
                return K_LEFT;
            return K_ESC;
        } else {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            return K_ESC;
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    if (c == '\n')
        return K_ENTER;
    if (c == 127 || c == 8)
        return K_BACK;
    return K_NONE;
#endif
}

void MainUI::renderFrame(int rows, int cols,
                         int &Lr1, int &Lc1, int &Lr2, int &Lc2,
                         int &Rr1, int &Rc1, int &Rr2, int &Rc2) {
    int header_h = 3;
    drawBox(1, 1, header_h, cols);
    gotoRC(1, 3);
    bold();
    cout << "POS Console - MainUI (Admin | Content)";
    reset();
    gotoRC(2, 3);
    gray();
    cout << "Left/Right: focus  |  Up/Down: move  |  Enter: select  |  Backspace: back  |  Esc: quit";
    reset();

    int left_w = 26;
    Lr1 = header_h + 1;
    Lc1 = 1;
    Lr2 = rows - 1;
    Lc2 = left_w + 1;
    drawBox(Lr1, Lc1, Lr2, Lc2);
    gotoRC(Lr1, Lc1 + 2);
    bold();
    cout << " ADMIN ";
    reset();

    Rr1 = header_h + 1;
    Rc1 = Lc2 + 1;
    Rr2 = rows - 1;
    Rc2 = cols;
    drawBox(Rr1, Rc1, Rr2, Rc2);
}
void MainUI::renderRightTitle(int r1, int c1, const string &title) {
    gotoRC(r1, c1 + 2);
    bold();
    cout << " " << title << " ";
    reset();
}

void MainUI::handleLeftKey(Key k, bool &moduleChanged) {
    int before = _adminUI.selectedIndex();
    if (k == K_UP)
        _adminUI.moveUp();
    if (k == K_DOWN)
        _adminUI.moveDown();
    if (k == K_RIGHT || k == K_ENTER)
        _focus = F_RIGHT;
    moduleChanged = (_adminUI.selectedIndex() != before);
}
void MainUI::handleRightKey(Key k) {
    int sel = _adminUI.selectedIndex(); // 0: Food, 1: Category
    if (k == K_LEFT) {
        _focus = F_LEFT;
        return;
    }
    if (sel == 0) {
        if (k == K_UP)
            _foodUI.onUp();
        if (k == K_DOWN)
            _foodUI.onDown();
        if (k == K_ENTER)
            _foodUI.onEnter();
        if (k == K_BACK)
            _foodUI.onBack();
    } else {
        if (k == K_UP)
            _categoryUI.onUp();
        if (k == K_DOWN)
            _categoryUI.onDown();
        if (k == K_ENTER)
            _categoryUI.onEnter();
        if (k == K_BACK)
            _categoryUI.onBack();
    }
}

void MainUI::onResized() {
    _adminUI.onResize();
    _foodUI.onResize();
    _categoryUI.onResize();
}

void MainUI::run() {
    enableVT();

    while (_running) {
        int rows, cols;
        termSize(rows, cols);
        bool resized = (rows != _prevRows || cols != _prevCols);
        _prevRows = rows;
        _prevCols = cols;

        int Lr1, Lc1, Lr2, Lc2, Rr1, Rc1, Rr2, Rc2;

        if (_firstFrame || resized) {
            hideCursor();
            clear();
            home();
            renderFrame(rows, cols, Lr1, Lc1, Lr2, Lc2, Rr1, Rc1, Rr2, Rc2);
            onResized();

            // LEFT full
            _adminUI.renderFull(Lr1, Lc1, Lr2, Lc2, _focus == F_LEFT);

            // RIGHT full (theo module hiện tại)
            _lastModule = _adminUI.selectedIndex();
            if (_lastModule == 0) {
                renderRightTitle(Rr1, Rc1, _foodUI.title());
                _foodUI.renderFull(Rr1, Rc1, Rr2, Rc2, _focus == F_RIGHT);
            } else {
                renderRightTitle(Rr1, Rc1, _categoryUI.title());
                _categoryUI.renderFull(Rr1, Rc1, Rr2, Rc2, _focus == F_RIGHT);
            }

            showCursor();
            _firstFrame = false;
        } else {
            renderFrame(rows, cols, Lr1, Lc1, Lr2, Lc2, Rr1, Rc1, Rr2, Rc2);
            renderRightTitle(Rr1, Rc1,
                             _adminUI.selectedIndex() == 0 ? _foodUI.title() : _categoryUI.title());
        }

        // ----- đọc phím & xử lý -----
        Key k = readKey();
        if (k == K_ESC) {
            _running = false;
            break;
        }

        bool moduleChanged = false;
        if (_focus == F_LEFT)
            handleLeftKey(k, moduleChanged);
        else
            handleRightKey(k);

        // ----- cập nhật UI theo thay đổi -----
        hideCursor();

        // Trái: delta (highlight đổi dòng/đổi active)
        _adminUI.renderDelta(Lr1, Lc1, Lr2, Lc2, _focus == F_LEFT);

        // Nếu module đổi → reset view của pane phải + full redraw
        int nowModule = _adminUI.selectedIndex();
        if (moduleChanged) {
            if (nowModule == 0) {
                _foodUI.resetView();
                renderRightTitle(Rr1, Rc1, _foodUI.title());
                _foodUI.renderFull(Rr1, Rc1, Rr2, Rc2, _focus == F_RIGHT);
            } else {
                _categoryUI.resetView();
                renderRightTitle(Rr1, Rc1, _categoryUI.title());
                _categoryUI.renderFull(Rr1, Rc1, Rr2, Rc2, _focus == F_RIGHT);
            }
            _lastModule = nowModule;
        } else {
            // Không đổi module → delta cho pane phải
            if (nowModule == 0) {
                renderRightTitle(Rr1, Rc1, _foodUI.title()); // rẻ, chỉ ghi 1 dòng
                _foodUI.renderDelta(Rr1, Rc1, Rr2, Rc2, _focus == F_RIGHT);
            } else {
                renderRightTitle(Rr1, Rc1, _categoryUI.title());
                _categoryUI.renderDelta(Rr1, Rc1, Rr2, Rc2, _focus == F_RIGHT);
            }
        }

        showCursor();
    }

    clear();
    home();
    cout << "Bye!\n";
}

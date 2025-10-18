#include "ConsoleUI.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring> // strlen

using namespace std;

// =======================================================
//                MINI FONT 5 DÒNG (C++98)
// =======================================================
static const char* SPCS[5] = { "  ","  ","  ","  ","  " };

static const char* A5[5] = {
  " *** ",
  "*   *",
  "*****",
  "*   *",
  "*   *"
};
static const char* B5[5] = {
  "**** ",
  "*   *",
  "**** ",
  "*   *",
  "**** "
};
static const char* E5[5] = {
  "*****",
  "*    ",
  "**** ",
  "*    ",
  "*****"
};
static const char* H5[5] = {
  "*   *",
  "*   *",
  "*****",
  "*   *",
  "*   *"
};
static const char* L5[5] = {
  "*    ",
  "*    ",
  "*    ",
  "*    ",
  "*****"
};
static const char* N5[5] = {
  "*   *",
  "**  *",
  "* * *",
  "*  **",
  "*   *"
};
static const char* O5[5] = {
  " *** ",
  "*   *",
  "*   *",
  "*   *",
  " *** "
};
static const char* P5[5] = {
  "**** ",
  "*   *",
  "**** ",
  "*    ",
  "*    "
};
static const char* R5[5] = {
  "**** ",
  "*   *",
  "**** ",
  "*  * ",
  "*   *"
};
static const char* S5[5] = {
  " ****",
  "*    ",
  " *** ",
  "    *",
  "**** "
};
static const char* T5[5] = {
  "*****",
  "  *  ",
  "  *  ",
  "  *  ",
  "  *  "
};
static const char* U5[5] = {
  "*   *",
  "*   *",
  "*   *",
  "*   *",
  " *** "
};
static const char* Y5[5] = {
  "*   *",
  " * * ",
  "  *  ",
  "  *  ",
  "  *  "
};
static const char* Z2[5] = { // s? 2
  " *** ",
  "*   *",
  "   * ",
  "  *  ",
  "*****"
};

static const char* const* pick(char c){
    if (c>='a' && c<='z') c = char(c - 'a' + 'A');
    switch(c){
        case 'A': return A5; case 'B': return B5; case 'E': return E5;
        case 'H': return H5; case 'L': return L5; case 'N': return N5;
        case 'O': return O5; case 'P': return P5; case 'R': return R5;
        case 'S': return S5; case 'T': return T5; case 'U': return U5;
        case 'Y': return Y5; case '2': return Z2; case ' ': return SPCS;
        default:  return SPCS;
    }
}

// ---- Helpers ----
static void appendScaledRowToString(const char* row, int scaleX, std::string& out) {
    for (const char* p = row; *p; ++p) {
        char ch = *p;
        if (ch == '*') {
            for (int i = 0; i < scaleX; ++i) out.push_back('*');
        } else {
            for (int i = 0; i < scaleX; ++i) out.push_back(' ');
        }
    }
}

static void buildBigLineRows(const std::string& s, int scaleX, std::vector<std::string>& rowsOut) {
    const int ROWS = 5;
    rowsOut.clear();
    rowsOut.resize(ROWS);
    for (int r = 0; r < ROWS; ++r) {
        std::string line;
        for (size_t i = 0; i < s.size(); ++i) {
            const char* const* g = pick(s[i]);
            appendScaledRowToString(g[r], scaleX, line);
            if (i + 1 < s.size()) line.push_back(' '); // kho?ng cách gi?a ký t?
        }
        rowsOut[r] = line;
    }
}

namespace ConsoleUI {

    void clearAll() {
    #ifdef _WIN32
        system("cls");
    #else
        std::cout << "\x1b[2J\x1b[H";
    #endif
        std::cout.flush();
    }

    void printBigLine(const std::string& s) {
        std::vector<std::string> rows;
        buildBigLineRows(s, /*scaleX=*/1, rows);
        for (size_t i = 0; i < rows.size(); ++i) {
            std::cout << rows[i] << "\n";
        }
    }

    // Banner + KHUNG v?i: padding 2 kho?ng, d?c "||", ngang "="
    void printAsciiBanner() {
        const int SCALE  = 1;
        const int PAD    = 2;            // <-- cách 2 kho?ng m?i bên
        const std::string SIDE = "||";   // <-- vi?n d?c
        // 1) Render hai dòng
        std::vector<std::string> pbl2Rows;
        std::vector<std::string> restRows;
        buildBigLineRows("PBL2", SCALE, pbl2Rows);
        buildBigLineRows("RESTAURANT", SCALE, restRows);

        // 2) Ð? r?ng n?i dung l?n nh?t
        size_t inner_w = 0;
        for (int r = 0; r < 5; ++r) {
            if (pbl2Rows[r].size() > inner_w) inner_w = pbl2Rows[r].size();
            if (restRows[r].size() > inner_w) inner_w = restRows[r].size();
        }

        // 3) T?ng chi?u r?ng 1 dòng có vi?n:
        //    = SIDE + pad + content(inner_w) + pad + SIDE
        const size_t line_w = SIDE.size() + PAD + inner_w + PAD + SIDE.size();

        // 4) Vi?n ngang b?ng '=' dúng dúng chi?u r?ng dòng
        std::string hbar(line_w, '=');

        // ---- In khung ----
        std::cout << hbar << "\n";

        // PBL2 (5 dòng)
        for (int r = 0; r < 5; ++r) {
            std::string line = pbl2Rows[r];
            if (line.size() < inner_w) line.append(inner_w - line.size(), ' ');
            std::cout << SIDE
                      << std::string(PAD, ' ')
                      << line
                      << std::string(PAD, ' ')
                      << SIDE << "\n";
        }

        // Dòng tr?ng ngan gi?a hai c?m
        std::cout << SIDE
                  << std::string(PAD + inner_w + PAD, ' ')
                  << SIDE << "\n";

        // RESTAURANT (5 dòng)
        for (int r = 0; r < 5; ++r) {
            std::string line = restRows[r];
            if (line.size() < inner_w) line.append(inner_w - line.size(), ' ');
            std::cout << SIDE
                      << std::string(PAD, ' ')
                      << line
                      << std::string(PAD, ' ')
                      << SIDE << "\n";
        }

        std::cout << hbar << "\n\n";
        std::cout.flush();
    }
}


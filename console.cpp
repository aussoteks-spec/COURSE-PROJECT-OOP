#include "console.h"

using namespace std;

void Console::setDefaultColors(int text, int bg) {
    defaultTextColor.set(text);
    defaultBgColor.set(bg);
}

int Console::getDefaultTextColor() const { return defaultTextColor.get(); }
int Console::getDefaultBgColor() const { return defaultBgColor.get(); }
bool Console::isCursorVisible() const { return cursorVisible.get(); }

void Console::setCursorVisible(bool visible) {
    cursorVisible.set(visible);
    if (visible) showCursor();
    else hideCursor();
}

void Console::setColor(int textColor, int bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
}

void Console::setCursorPos(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void Console::hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void Console::showCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void Console::getConsoleSize(int& width, int& height) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void Console::applyDefaultSettings() {
    setColor(defaultTextColor.get(), defaultBgColor.get());
    if (cursorVisible.get()) {
        showCursor();
    }
    else {
        hideCursor();
    }
}

int Console::showMenu(const vector<string>& items, const string& title) {
    system("cls");

    int consoleWidth, consoleHeight;
    getConsoleSize(consoleWidth, consoleHeight);

    int menuHeight = items.size() + 4;
    int startY = (consoleHeight - menuHeight) / 2;

    for (int i = 0; i < startY; i++) {
        cout << endl;
    }

    int titlePadding = (consoleWidth - title.length()) / 2;
    cout << string(titlePadding, ' ') << title << "\n\n";

    for (int i = 0; i < items.size(); i++) {
        int itemPadding = (consoleWidth - items[i].length() - 2) / 2;
        cout << string(itemPadding, ' ') << ' ' << items[i] << '\n';
    }

    int selected = 0;

    while (true) {
        setCursorPos(0, startY + 2);

        for (int i = 0; i < items.size(); i++) {
            int itemPadding = (consoleWidth - items[i].length() - 2) / 2;
            setCursorPos(itemPadding, startY + 2 + i);

            if (i == selected) {
                setColor(14);
                cout << '>' << items[i];
                setColor(7);
            }
            else {
                setColor(7);
                cout << ' ' << items[i] << ' ';
            }
        }

        int ch = _getch();
        if (ch == 224) {
            ch = _getch();
            if (ch == 72) {
                selected = (selected - 1 + items.size()) % items.size();
            }
            else if (ch == 80) {
                selected = (selected + 1) % items.size();
            }
        }
        else if (ch == 13) {
            return selected;
        }
        else if (ch == 27) {
            return -1;
        }
    }
}
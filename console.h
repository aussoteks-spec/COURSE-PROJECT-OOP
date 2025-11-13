#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <map>
#include <conio.h>
#include <sstream>
#include <fstream>
#include <memory>
#include <iomanip>

using namespace std;

class Console {
public:
    static void setColor(int textColor, int bgColor = 0) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
    }

    static void setCursorPos(int x, int y) {
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    static void hideCursor() {
        CONSOLE_CURSOR_INFO cursorInfo;
        cursorInfo.dwSize = 1;
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    }

    static void showCursor() {
        CONSOLE_CURSOR_INFO cursorInfo;
        cursorInfo.dwSize = 1;
        cursorInfo.bVisible = TRUE;
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    }

    static void getConsoleSize(int& width, int& height) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }

    int showMenu(const vector<string>& items, const string& title = "лемч") {
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
};
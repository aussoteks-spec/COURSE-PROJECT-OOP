#pragma once
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <conio.h>
#include <sstream>
#include <memory>
#include <iomanip>

using namespace std;

template<typename T>
class ValueCon {
private:
    T value;
public:
    ValueCon(const T& val) : value(val) {}
    T get() const { return value; }
    void set(const T& val) { value = val; }
    void print() const {
        cout << value << endl;
    }
};

class Console {
private:
    ValueCon<int> defaultTextColor{ 7 };
    ValueCon<int> defaultBgColor{ 0 };
    ValueCon<bool> cursorVisible{ false };

public:
    void setDefaultColors(int text, int bg);
    int getDefaultTextColor() const;
    int getDefaultBgColor() const;
    bool isCursorVisible() const;
    void setCursorVisible(bool visible);
    static void setColor(int textColor, int bgColor = 0);
    static void setCursorPos(int x, int y);
    static void hideCursor();
    static void showCursor();
    static void getConsoleSize(int& width, int& height);
    void applyDefaultSettings();
    int showMenu(const vector<string>& items, const string& title = "лемч");
};
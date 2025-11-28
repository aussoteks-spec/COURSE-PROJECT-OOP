#include "table.h"
using namespace std;

namespace TablePrinter {
    //рисует горизонтальную границу таблицы
    void printBorder() {
        cout << "+" << string(WIDTH - 2, '-') << "+" << endl;
    }
    //создаёт пустую строку внутри рамки
    void printEmptyLineInBox() {
        cout << "|" << string(WIDTH - 2, ' ') << "|" << endl;
    }
    //выводит текст в центре рамки
    void printCenteredInBox(const string& text) {
        int inner = WIDTH - 2;
        string t = text;
        if ((int)t.size() > inner) {
            t = t.substr(0, inner - 3) + "...";
        }
        int leftPad = (inner - (int)t.size()) / 2;
        int rightPad = inner - leftPad - (int)t.size();
        cout << "|" << string(leftPad, ' ') << t << string(rightPad, ' ') << "|" << endl;
    }
    //выводит строку в центре без рамки
    void printCenteredLine(const string& text) {
        int padding = (WIDTH - static_cast<int>(text.size())) / 2;
        if (padding < 0) padding = 0;
        cout << string(padding, ' ') << text << "\n";
    }
    //выводит текст в центре
    void printCentered(const string& text) {
        int padding = (WIDTH - static_cast<int>(text.size())) / 2;
        if (padding < 0) padding = 0;
        cout << string(padding, ' ') << text;
    }
    //функция для записи текста в центре ячейки
    string centerCell(const string& text, int width) {
        if (text.length() >= width) return text.substr(0, width);
        int padding = width - text.length();
        int left = padding / 2;
        int right = padding - left;
        return string(left, ' ') + text + string(right, ' ');
    }
    //выводит шапку таблицы
    void printDoctorScheduleHeader(const string& doctorName, const string& specialization) {
        printBorder();
        printCenteredInBox("РАСПИСАНИЕ ВРАЧА");
        printBorder();
        printCenteredInBox("Врач: " + doctorName);
        printCenteredInBox("Специализация: " + specialization);
        printBorder();
    }
    //выводит расписание врача в таблице
    void printScheduleTable(const vector<pair<string, bool>>& schedule) {
        cout << "+" << string(25, '-') << "+" << string(15, '-') << "+" << endl;
        cout << "|" << centerCell("ВРЕМЯ ПРИЕМА", 25)
            << "|" << centerCell("СТАТУС", 15) << "|" << endl;
        cout << "+" << string(25, '-') << "+" << string(15, '-') << "+" << endl;
        for (const auto& slot : schedule) {
            string time = slot.first;
            string status = slot.second ? "СВОБОДНО" : "ЗАНЯТО";
            string statusColor = slot.second ? "\033[32m" : "\033[31m"; 

            cout << "| " << left << setw(24) << time
                << "| " << statusColor << setw(13) << status << "\033[0m|" << endl;
        }

        cout << "+" << string(25, '-') << "+" << string(15, '-') << "+" << endl;

        int freeSlots = 0;
        for (const auto& slot : schedule) {
            if (slot.second) freeSlots++;
        }

        cout << "| " << left << setw(24) << "Всего: " + to_string(schedule.size())
            << "| " << setw(13) << "Свободно: " + to_string(freeSlots) << "|" << endl;
        cout << "+" << string(25, '-') << "+" << string(15, '-') << "+" << endl;
    }
}
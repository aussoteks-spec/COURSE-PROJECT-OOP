#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

namespace TablePrinter {
    static const int WIDTH = 80;

    void printBorder();
    void printEmptyLineInBox();
    void printCenteredInBox(const std::string& text);
    void printCenteredLine(const std::string& text);
    void printCentered(const std::string& text);
    std::string centerCell(const std::string& text, int width);

    void printDoctorScheduleHeader(const std::string& doctorName, const std::string& specialization);
    void printScheduleTable(const std::vector<std::pair<std::string, bool>>& schedule);
}
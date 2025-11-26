#include "table.h"

namespace TablePrinter {

    void printBorder() {
        std::cout << "+" << std::string(WIDTH - 2, '-') << "+" << "\n";
    }

    void printEmptyLineInBox() {
        std::cout << "|" << std::string(WIDTH - 2, ' ') << "|" << "\n";
    }

    void printCenteredInBox(const std::string& text) {
        int inner = WIDTH - 2;
        std::string t = text;
        if ((int)t.size() > inner) {
            t = t.substr(0, inner - 3) + "...";
        }
        int leftPad = (inner - (int)t.size()) / 2;
        int rightPad = inner - leftPad - (int)t.size();
        std::cout << "|" << std::string(leftPad, ' ') << t << std::string(rightPad, ' ') << "|" << "\n";
    }

    void printCenteredLine(const std::string& text) {
        int padding = (WIDTH - static_cast<int>(text.size())) / 2;
        if (padding < 0) padding = 0;
        std::cout << std::string(padding, ' ') << text << "\n";
    }

    void printCentered(const std::string& text) {
        int padding = (WIDTH - static_cast<int>(text.size())) / 2;
        if (padding < 0) padding = 0;
        std::cout << std::string(padding, ' ') << text;
    }

    std::string centerCell(const std::string& text, int width) {
        if (text.length() >= width) return text.substr(0, width);
        int padding = width - text.length();
        int left = padding / 2;
        int right = padding - left;
        return std::string(left, ' ') + text + std::string(right, ' ');
    }

    void printDoctorScheduleHeader(const std::string& doctorName, const std::string& specialization) {
        printBorder();
        printCenteredInBox("ĞÀÑÏÈÑÀÍÈÅ ÂĞÀ×À");
        printBorder();
        printCenteredInBox("Âğà÷: " + doctorName);
        printCenteredInBox("Ñïåöèàëèçàöèÿ: " + specialization);
        printBorder();
    }

    void printScheduleTable(const std::vector<std::pair<std::string, bool>>& schedule) {
        std::cout << "+" << std::string(25, '-') << "+" << std::string(15, '-') << "+" << "\n";
        std::cout << "|" << centerCell("ÂĞÅÌß ÏĞÈÅÌÀ", 25)
            << "|" << centerCell("ÑÒÀÒÓÑ", 15) << "|" << "\n";
        std::cout << "+" << std::string(25, '-') << "+" << std::string(15, '-') << "+" << "\n";
        for (const auto& slot : schedule) {
            std::string time = slot.first;
            std::string status = slot.second ? "ÑÂÎÁÎÄÍÎ" : "ÇÀÍßÒÎ";
            std::string statusColor = slot.second ? "\033[32m" : "\033[31m"; 

            std::cout << "| " << std::left << std::setw(24) << time
                << "| " << statusColor << std::setw(13) << status << "\033[0m|" << "\n";
        }

        std::cout << "+" << std::string(25, '-') << "+" << std::string(15, '-') << "+" << "\n";

        int freeSlots = 0;
        for (const auto& slot : schedule) {
            if (slot.second) freeSlots++;
        }

        std::cout << "| " << std::left << std::setw(24) << "Âñåãî: " + std::to_string(schedule.size())
            << "| " << std::setw(13) << "Ñâîáîäíî: " + std::to_string(freeSlots) << "|" << "\n";
        std::cout << "+" << std::string(25, '-') << "+" << std::string(15, '-') << "+" << "\n";
    }
}
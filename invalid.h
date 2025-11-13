#pragma once
#include "patient.h"
#include <fstream>

class Invalid : public Patient {
    int disabilityGroup;
public:
    Invalid() : disabilityGroup(0) {}
    Invalid(Person* p, const string& ill, int g) : Patient(p, ill), disabilityGroup(g) {}

    void show() override {
        cout << "\nПациент (инвалид)\n";
        if (info) info->showInfo();
        cout << "Заболевание: " << illness << "\nГруппа инвалидности: " << disabilityGroup << "\n";
    }

    void edit() override {
        Patient::edit();
        cout << "Группа инвалидности: "; cin >> disabilityGroup; cin.ignore();
    }
};

#pragma once
#include "patient.h"
#include <fstream>

class Child : public Patient {
    string parentName;
public:
    Child() : parentName("") {}
    Child(Person* p, const string& ill, const string& parent) : Patient(p, ill), parentName(parent) {}

    void show() override {
        cout << "\n=== Пациент (ребёнок) ===\n";
        if (info) info->showInfo();
        cout << "Заболевание: " << illness << "\nРодитель/опекун: " << parentName << "\n";
    }

    void edit() override {
        Patient::edit();
        cout << "Имя родителя/опекуна: ";
        getline(cin >> ws, parentName);
    }
};
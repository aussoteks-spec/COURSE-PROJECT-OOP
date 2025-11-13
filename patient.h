#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "infoPatient.h"
using namespace std;

class Patient : public InfoPatient {
private:
    vector<string> visitHistory;

public:
    Patient() : InfoPatient() {}
    Patient(Person* p, const string& ill) : InfoPatient(p, ill) {}

    void show() override {
        info->showInfo();
        cout << "Заболевание: " << illness << endl;
    }

    virtual void edit() {
        cout << "\nРедактирование пациента:\n";
        Person* p = new Person();
        p->inputInfo();
        cout << "Заболевание: ";
        string ill; getline(cin >> ws, ill);
        info.reset(p);
        illness = ill;
        cout << "Данные обновлены.\n";
    }

    void addVisit(const string& record) {
        visitHistory.push_back(record);
    }

    void showHistory() const {
        cout << "\nИстория посещений\n";
        if (visitHistory.empty()) {
            cout << "Нет записей о посещениях.\n";
            return;
        }
        for (const auto& rec : visitHistory)
            cout << "- " << rec << endl;
    }
};
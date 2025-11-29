#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "infopatient.h"
using namespace std;

class Patient : public InfoPatient {
private:
    vector<string> visitHistory;
public:
    Patient();
    Patient(Person* p, const string& illÿ);

    virtual void show() override;
    virtual void edit();

    void addVisit(const string& record);
    void showHistory() const;

    friend ostream& operator<<(ostream& os, const Patient& p);
};
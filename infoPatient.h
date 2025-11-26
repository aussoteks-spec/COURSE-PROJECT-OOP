#pragma once
#include <memory>
#include <string>
#include "info.h"

using namespace std;

class InfoPatient {
protected:
    unique_ptr<Person> info;
    string illness;
public:
    InfoPatient();
    InfoPatient(Person* p, const string& ill);
    virtual ~InfoPatient() = default;

    virtual void show() = 0;

    string getIllness() const;
    string getInfoString() const;
    string getFullName() const;

    void setInfo(Person* p);
    void setIllness(const string& s);
};
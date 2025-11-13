#pragma once
#include <memory>
#include "info.h"
#include <string>
using namespace std;

class InfoPatient {
protected:
    unique_ptr<Person> info;
    string illness;
public:
    InfoPatient() : info(make_unique<Person>()), illness("Не указано") {}
    InfoPatient(Person* p, const string& ill) : info(p), illness(ill) {}
    virtual ~InfoPatient() = default;
    virtual void show() = 0;

    string getIllness() const { return illness; }
    string getInfoString() const { return info ? info->serializeCompact() : string(); }
    string getFullName() const { return info ? info->getFullName() : string(); }

    void setInfo(Person* p) { info.reset(p); }
    void setIllness(const string& s) { illness = s; }
};
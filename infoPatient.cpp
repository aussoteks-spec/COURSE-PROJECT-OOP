#include "infopatient.h"

using namespace std;

InfoPatient::InfoPatient() : info(make_unique<Person>()), illness("не указан") {}

InfoPatient::InfoPatient(Person* p, const string& ill) : info(p), illness(ill) {}

string InfoPatient::getIllness() const {
    return illness;
}

string InfoPatient::getInfoString() const {
    return info ? info->serializeCompact() : string();
}

string InfoPatient::getFullName() const {
    return info ? info->getFullName() : string();
}

void InfoPatient::setInfo(Person* p) {
    info.reset(p);
}

void InfoPatient::setIllness(const string& s) {
    illness = s;
}
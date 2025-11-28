#include "infopatient.h"

using namespace std;

InfoPatient::InfoPatient() : info(make_unique<Person>()), illness("не указан") {}

InfoPatient::InfoPatient(Person* p, const string& ill) : info(p), illness(ill) {}
//возвращает заболевание пациента
string InfoPatient::getIllness() const {
    return illness;
}
//возвращает сериализованные данные
string InfoPatient::getInfoString() const {
    return info ? info->serializeCompact() : string();
}
//возвращает полное фио пациента
string InfoPatient::getFullName() const {
    return info ? info->getFullName() : string();
}
//изменяет заболевание пациента
void InfoPatient::setIllness(const string& s) {
    illness = s;
}
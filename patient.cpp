#include "patient.h"
#include <sstream>
#include <stdexcept>

using namespace std;

Patient::Patient() : InfoPatient() {}

Patient::Patient(Person* p, const string& ill) : InfoPatient(p, ill) {}
//вывод информации о пациенте (обычном) на экран
void Patient::show() {
    try {
        cout << "Пациент (обычный)\n";
        cout << "ФИО: " << getFullName() << "\n";
        cout << "Возраст: " << info->getAge() << "\n";
        cout << "Пол: " << info->getGender() << "\n";
        cout << "Телефон: " << info->getPhone() << "\n";
        cout << "Адрес: " << info->getAddress() << "\n";
        cout << "Диагноз: " << illness << endl;
    }
    catch (const exception& e) {
        cout << "Ошибка отображения данных пациента: " << e.what() << endl;
    }
}
//редактирует данные пациента (обычный)
void Patient::edit() {
    try {
        cout << "\nРедактирование пациента:\n";
        Person* p = new Person();
        p->inputInfo();

        cout << "Диагноз: ";
        string ill;
        getline(cin, ill);

        if (ill.empty()) {
            throw invalid_argument("Диагноз не может быть пустым");
        }

        info.reset(p);
        setIllness(ill);
        cout << "Данные пациента обновлены.\n";
    }
    catch (const exception& e) {
        throw;
    }
}
//добавляет запись о посещении в историю пациента
void Patient::addVisit(const string& record) {
    try {
        if (record.empty()) {
            throw invalid_argument("Запись посещения не может быть пустой");
        }
        visitHistory.push_back(record);
    }
    catch (const exception& e) {
        cout << "Ошибка добавления записи посещения: " << e.what() << endl;
        throw;
    }
}
//показывает изторию пациента
void Patient::showHistory() const {
    try {
        cout << "\nИстория посещений\n";
        if (visitHistory.empty()) {
            cout << "Записей в истории нет.\n";
            return;
        }
        for (const auto& rec : visitHistory) {
            cout << "- " << rec << endl;
        }
    }
    catch (const exception& e) {
        cout << "Ошибка отображения истории посещений: " << e.what() << endl;
    }
}

ostream& operator<<(ostream& os, const Patient& p) {
    os << "ФИО: " << p.getFullName() << endl;
    os << "Возраст: " << p.info->getAge() << endl;
    os << "Пол: " << p.info->getGender() << endl;
    os << "Телефон: " << p.info->getPhone() << endl;
    os << "Адрес: " << p.info->getAddress() << endl;
    os << "Диагноз: " << p.illness << endl;
    return os;
}
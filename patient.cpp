#include "patient.h"
#include <sstream>
#include <stdexcept>

using namespace std;

Patient::Patient() : InfoPatient() {}

Patient::Patient(Person* p, const string& ill) : InfoPatient(p, ill) {}

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

void Patient::edit() {
    try {
        cout << "\nРедактирование пациента:\n";
        Person* p = new Person();
        p->inputInfo();

        cout << "Диагноз: ";
        string ill;
        getline(cin >> ws, ill);

        if (ill.empty()) {
            throw invalid_argument("Диагноз не может быть пустым");
        }

        info.reset(p);
        illness = ill;
        cout << "Данные пациента обновлены.\n";
    }
    catch (const exception& e) {
        throw runtime_error("Ошибка редактирования пациента");
    }
}

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

const vector<string>& Patient::getVisitHistory() const {
    return visitHistory;
}

ostream& operator<<(ostream& os, const Patient& p) {
    os << "ФИО: " << p.getFullName() << "\n";
    os << "Возраст: " << p.info->getAge() << "\n";
    os << "Пол: " << p.info->getGender() << "\n";
    os << "Телефон: " << p.info->getPhone() << "\n";
    os << "Адрес: " << p.info->getAddress() << "\n";
    os << "Диагноз: " << p.illness << "\n";
    return os;
}
#include "invalid.h"
#include <iostream>

using namespace std;

Invalid::Invalid() : disabilityGroup(0) {}

Invalid::Invalid(Person* p, const string& ill, int g) : Patient(p, ill), disabilityGroup(g) {}

int Invalid::getDisabilityGroup() const {
    return disabilityGroup;
}

void Invalid::show() {
    try {
        cout << "\nПациент (инвалид)\n";
        cout << "ФИО: " << getFullName() << endl;
        cout << "Возраст: " << info->getAge() << endl;
        cout << "Пол: " << info->getGender() << endl;
        cout << "Телефон: " << info->getPhone() << endl;
        cout << "Адрес: " << info->getAddress() << endl;
        cout << "Диагноз: " << illness << endl;
        cout << "Группа инвалидности: " << disabilityGroup << endl;
    }
    catch (const exception& e) {
        cout << "Ошибка отображения данных пациента-инвалида: " << e.what() << endl;
    }
}

void Invalid::edit() {
    try {
        Patient::edit();

        cout << "Группа инвалидности: ";
        if (!(cin >> disabilityGroup)) {
            cin.clear();
            string temp;
            getline(cin, temp);
            throw invalid_argument("Группа инвалидности должна быть числом");
        }

        if (disabilityGroup < 1 || disabilityGroup > 3) {
            throw invalid_argument("Группа инвалидности должна быть от 1 до 3");
        }

        cin.ignore(1000, '\n');
        cout << "Группа инвалидности обновлена.\n";
    }
    catch (const exception& e) {
        throw runtime_error("Ошибка редактирования пациента-инвалида");
    }
}
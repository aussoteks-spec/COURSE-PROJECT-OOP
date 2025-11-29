#include "child.h"
using namespace std;

Child::Child() : Patient(), parentName("") {}

Child::Child(Person* p, const string& ill, const string& parent)
    : Patient(p, ill), parentName(parent) {
}
//возвращает фио опекуна/родителя пациента 
string Child::getParentName() const {
    return parentName;
}
//вывод информации о пациенте (ребёнок) на экран
void Child::show() {
    try {
        cout << "\nПациент (ребенок) \n";
        cout << "ФИО: " << getFullName() << endl;
        cout << "Возраст: " << info->getAge() << endl;
        cout << "Пол: " << info->getGender() << endl;
        cout << "Телефон: " << info->getPhone() << endl;
        cout << "Адрес: " << info->getAddress() << endl;
        cout << "Диагноз: " << illness << endl;
        cout << "Родитель/опекун: " << parentName << endl;
    }
    catch (const exception& e) {
        cout << "Ошибка отображения данных ребенка: " << e.what() << endl;
    }
}
//редактирует данные пациента (ребёнок)
void Child::edit() {
    try {
        Patient::edit();

        cout << "Имя родителя/опекуна: ";
        getline(cin, parentName);

        if (parentName.empty()) {
            throw invalid_argument("Имя родителя/опекуна не может быть пустым");
        }

        cout << "Данные родителя/опекуна обновлены.\n";
    }
    catch (const exception& e) {
        throw;
    }
}
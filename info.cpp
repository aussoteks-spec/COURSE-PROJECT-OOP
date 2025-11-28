#include "info.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cctype>

using namespace std;
//проверяет корректность номера телефона
bool Person::isValidPhone(const string& phone) {
    if (phone.length() != 13) return false;
    if (phone[0] != '+' || phone[1] != '3' || phone[2] != '7' || phone[3] != '5') return false;

    for (size_t i = 4; i < phone.length(); ++i) {
        if (!isdigit(phone[i])) return false;
    }

    string operatorCode = phone.substr(4, 2);
    vector<string> validOperators = { "29", "33", "44", "25", "17", "15" };
    return find(validOperators.begin(), validOperators.end(), operatorCode) != validOperators.end();
}

Person::Person() : surname(""), name(""), patronymic(""), age(0), gender(""), phone(""), address("") {}

Person::Person(string s, string n, string p, int a, string g, string ph, string ad)
    : surname(s), name(n), patronymic(p), age(a), gender(g), phone(ph), address(ad) {
}
//ввод данных с клавиатуры
void Person::inputInfo() {
    try {
        cout << "Фамилия: ";
        getline(cin >> ws, surname);
        if (surname.empty()) throw invalid_argument("Фамилия не может быть пустой");

        cout << "Имя: ";
        getline(cin, name);
        if (name.empty()) throw invalid_argument("Имя не может быть пустым");

        cout << "Отчество/матчество (если нет - Enter): ";
        getline(cin, patronymic);

        cout << "Возраст: ";
        if (!(cin >> age)) {
            cin.clear();
            string temp;
            getline(cin, temp);
            throw invalid_argument("Возраст должен быть числом");
        }
        cin.ignore(1000, '\n');

        cout << "Пол: ";
        getline(cin, gender);
        if (gender.empty()) throw invalid_argument("Пол не может быть пустым");

        while (true) {
            try {
                cout << "Телефон (+375XXXXXXXXX): ";
                getline(cin, phone);

                if (!isValidPhone(phone)) {
                    throw invalid_argument(
                        "Неверный формат телефона. Должен быть: +375XXXXXXXXX\n"
                        "Допустимые операторы: 29, 33, 44, 25, 17, 15\n"
                    );
                }
                break;
            }
            catch (const exception& e) {
                cerr << "Ошибка: " << e.what() << endl;
                cout << "Пожалуйста, введите номер заново: ";
            }
        }

        cout << "Адрес: ";
        getline(cin, address);
        if (address.empty()) throw invalid_argument("Адрес не может быть пустым");
    }
    catch (const exception& e) {
        throw;
    }
}
//вывод информации о человеке на экран
void Person::showInfo() const {
    cout << "ФИО: " << getFullName() << "\n";
    cout << "Возраст: " << age << "\n";
    cout << "Пол: " << gender << "\n";
    cout << "Телефон: " << phone << "\n";
    cout << "Адрес: " << address << "\n";
}
//формирует полное фио
string Person::getFullName() const {
    stringstream ss;
    ss << surname << " " << name;
    if (!patronymic.empty()) ss << " " << patronymic;
    return ss.str();
}
//сериализует объект в строку для сохранения в файл
string Person::serializeCompact() const {
    return surname + "|" + name + "|" + patronymic + "|" + to_string(age) + "|" + gender + "|" + phone + "|" + address;
}
//создает объект Person из сериализованной строки
Person Person::DeserializeFromFile(const string& s) {
    Person p;
    vector<string> parts;
    string cur;
    for (char c : s) {
        if (c == '|') { parts.push_back(cur); cur.clear(); }
        else cur.push_back(c);
    }
    parts.push_back(cur);
    if (parts.size() >= 7) {
        p.surname = parts[0];
        p.name = parts[1];
        p.patronymic = parts[2];
        try {
            p.age = stoi(parts[3]);
        }
        catch (const exception& e) {
            p.age = 0;
        }
        p.gender = parts[4];
        p.phone = parts[5];
        p.address = parts[6];
    }
    return p;
}

ostream& operator<<(ostream& os, const Person& p) {
    os << "ФИО: " << p.getFullName()
        << ", Возраст: " << p.age
        << ", Пол: " << p.gender
        << ", Телефон: " << p.phone
        << ", Адрес: " << p.address;
    return os;
}
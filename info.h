#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

class Person {
public:
    string surname;
    string name;
    string patronymic;
    int age;
    string gender;
    string phone;
    string address;

    Person() : surname(""), name(""), patronymic(""), age(0), gender(""), phone(""), address("") {}

    Person(string s, string n, string p, int a, string g, string ph, string ad)
        : surname(s), name(n), patronymic(p), age(a), gender(g), phone(ph), address(ad) {
    }

    void inputInfo() {
        cout << "Фамилия: "; getline(cin >> ws, surname);
        cout << "Имя: "; getline(cin, name);
        cout << "Отчество (если нет — Enter): "; getline(cin, patronymic);
        cout << "Возраст: "; cin >> age; cin.ignore();
        cout << "Пол: "; getline(cin, gender);
        cout << "Телефон: "; getline(cin, phone);
        cout << "Адрес: "; getline(cin, address);
    }

    void showInfo() const {
        cout << "ФИО: " << getFullName() << "\n";
        cout << "Возраст: " << age << "\nПол: " << gender << "\nТелефон: " << phone << "\nАдрес: " << address << "\n";
    }

    string getFullName() const {
        stringstream ss;
        ss << surname << " " << name;
        if (!patronymic.empty()) ss << " " << patronymic;
        return ss.str();
    }

    string serializeCompact() const {
        return surname + "|" + name + "|" + patronymic + "|" + to_string(age) + "|" + gender + "|" + phone + "|" + address;
    }

    static Person deserializeCompact(const string& s) {
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
            p.age = stoi(parts[3]);
            p.gender = parts[4];
            p.phone = parts[5];
            p.address = parts[6];
        }
        return p;
    }

    friend ostream& operator<<(ostream& os, const Person& p) {
        os << "ФИО: " << p.getFullName()
            << ", Возраст: " << p.age
            << ", Пол: " << p.gender
            << ", Телефон: " << p.phone
            << ", Адрес: " << p.address;
        return os;
    }
};
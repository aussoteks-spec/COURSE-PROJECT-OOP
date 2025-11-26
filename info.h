#pragma once
#include <string>
#include <iostream>
using namespace std;

class Person {
private:
    bool isValidPhone(const string& phone);
    string surname;
    string name;
    string patronymic;
    int age;
    string gender;
    string phone;
    string address;
public:
    Person();
    Person(string s, string n, string p, int a, string g, string ph, string ad);

    void inputInfo();
    void showInfo() const;
    string getFullName() const;
    string serializeCompact() const;
    static Person DeserializeFromFile(const string& s);
    string getSurname() const { return surname; }
    string getName() const { return name; }
    string getPatronymic() const { return patronymic; }
    int getAge() const { return age; }
    string getGender() const { return gender; }
    string getPhone() const { return phone; }
    string getAddress() const { return address; }

    void setSurname(const string& s) { surname = s; }
    void setName(const string& n) { name = n; }
    void setPatronymic(const string& p) { patronymic = p; }
    void setAge(int a) { age = a; }
    void setGender(const string& g) { gender = g; }
    void setPhone(const string& p) { phone = p; }
    void setAddress(const string& a) { address = a; }

    friend ostream& operator<<(ostream& os, const Person& p);
};
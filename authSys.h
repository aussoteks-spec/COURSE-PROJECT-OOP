#pragma once
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>
#include "info.h"
using namespace std;

string hashPassword(const string& password) {
    string salt = "medical_system_Ksenon_2025";
    string salted = password + salt;
    hash<string> hasher;
    size_t hv = hasher(salted);
    stringstream ss; ss << hex << hv;
    return ss.str();
}

struct User {
    string hash;
    string role;
    string infoCompact;
    string extra;
    string fullName;
};

class AuthSystem {
    map<string, User> users;
    string filename;
    string currentLogin;
public:
    AuthSystem(const string& file = "users.dat") : filename(file) {
        readFile(filename);
    }

    bool hasCurrent() const { return !currentLogin.empty(); }
    string getCurrentLogin() const { return currentLogin; }
    string getCurrentRole() const {
        if (currentLogin.empty()) return "";
        auto it = users.find(currentLogin);
        if (it == users.end()) return "";
        return it->second.role;
    }
    string getCurrentFullName() const {
        if (currentLogin.empty()) return "";
        auto it = users.find(currentLogin);
        if (it == users.end()) return "";
        return it->second.fullName;
    }

    void writeFile(const string& file) {
        ofstream f(file, ios::binary | ios::trunc);
        if (!f.is_open()) { cout << "Не удалось открыть файл users.dat для записи\n"; return; }
        int n = users.size();
        f.write((char*)&n, sizeof(n));
        for (auto& kv : users) {
            writeString(f, kv.first);
            writeString(f, kv.second.hash);
            writeString(f, kv.second.role);
            writeString(f, kv.second.infoCompact);
            writeString(f, kv.second.extra);
            writeString(f, kv.second.fullName);
        }
        f.close();
    }

    void readFile(const string& file) {
        users.clear();
        ifstream f(file, ios::binary);
        if (!f.is_open()) return;
        int n;
        f.read((char*)&n, sizeof(n));
        for (int i = 0; i < n; ++i) {
            string login = readString(f);
            string hash = readString(f);
            string role = readString(f);
            string info = readString(f);
            string extra = readString(f);
            string fullName = readString(f);
            users[login] = { hash, role, info, extra, fullName };
        }
        f.close();
    }

    void addAdmin(const string& login, const string& password) {
        for (auto& kv : users) if (kv.second.role == "admin") {
            cout << "Администратор уже существует\n"; return;
        }
        User u;
        u.hash = hashPassword(password);
        u.role = "admin";
        u.infoCompact = "";
        u.extra = "";
        u.fullName = "Администратор";
        users[login] = u;
        writeFile(filename);
        cout << "Администратор создан\n";
    }

    bool registerPatient(const string& login, const string& password) {
        if (users.count(login)) {
            cout << "Ошибка: логин уже существует\n";
            return false;
        }
        Person p;
        cout << "Заполните информацию о пациенте:\n";
        p.inputInfo();
        cout << "Категория (инвалид/ребёнок/иностранец/обычный): ";
        string category; getline(cin, category);
        string extra;
        if (category == "инвалид") {
            cout << "Введите группу инвалидности: "; getline(cin, extra);
        }
        else if (category == "ребёнок") {
            cout << "Введите имя родителя/опекуна: "; getline(cin, extra);
        }
        else if (category == "иностранец") {
            cout << "Введите страну/документы: "; getline(cin, extra);
        }
        User u;
        u.hash = hashPassword(password);
        u.role = "patient";
        u.infoCompact = p.serializeCompact();
        u.extra = category + (extra.empty() ? "" : "|" + extra);
        u.fullName = p.getFullName();
        users[login] = u;
        writeFile(filename);
        cout << "Пациент зарегистрирован\n";
        return true;
    }

    bool addDoctorByAdmin(const string& adminLogin, const string& doctorLogin, const string& password) {
        auto it = users.find(adminLogin);
        if (it == users.end() || it->second.role != "admin") {
            cout << "Только админ может добавлять врачей\n"; return false;
        }
        if (users.count(doctorLogin)) { cout << "Логин врача уже существует\n"; return false; }
        Person p;
        cout << "Введите данные врача:\n";
        p.inputInfo();
        cout << "Специализация: "; string spec; getline(cin, spec);
        cout << "Опыт (лет): "; int exp; cin >> exp; cin.ignore();

        User u;
        u.hash = hashPassword(password);
        u.role = "doctor";
        u.infoCompact = p.serializeCompact() + "|" + spec + "|" + to_string(exp);
        u.extra = spec;
        u.fullName = p.getFullName();
        users[doctorLogin] = u;
        writeFile(filename);
        cout << "Врач добавлен\n";
        return true;
    }

    pair<bool, string> authenticate(const string& login, const string& password) {
        auto it = users.find(login);
        if (it == users.end()) { cout << "Пользователь не найден\n"; return { false,"" }; }
        string h = hashPassword(password);
        if (h == it->second.hash) {
            currentLogin = login;
            cout << "Вход успешен. Роль: " << it->second.role << "\n";
            cout << "Информация:\n" << (it->second.infoCompact.empty() ? "(нет)" : it->second.infoCompact) << "\n";
            return { true, it->second.role };
        }
        else {
            cout << "Неверный пароль\n"; return { false,"" };
        }
    }

    void logout() { currentLogin.clear(); }

    bool getUserByLogin(const string& login, User& out) const {
        auto it = users.find(login);
        if (it == users.end()) return false;
        out = it->second; return true;
    }

    bool editPatientInfoByLogin(const string& login) {
        auto it = users.find(login);
        if (it == users.end()) { cout << "Пользователь не найден\n"; return false; }
        if (it->second.role != "patient") { cout << "Это не пациент\n"; return false; }
        Person p = Person::deserializeCompact(it->second.infoCompact);
        cout << "Текущие данные:\n"; p.showInfo();
        cout << "Введите новые данные:\n"; p.inputInfo();
        it->second.infoCompact = p.serializeCompact();
        it->second.fullName = p.getFullName();
        writeFile(filename);
        cout << "Данные обновлены\n";
        return true;
    }

    bool editDoctorInfoByLogin(const string& login) {
        auto it = users.find(login);
        if (it == users.end()) { cout << "Пользователь не найден\n"; return false; }
        if (it->second.role != "doctor") { cout << "Это не врач\n"; return false; }
        string compact = it->second.infoCompact;
        vector<string> parts;
        string cur;
        for (char c : compact) {
            if (c == '|') { parts.push_back(cur); cur.clear(); }
            else cur.push_back(c);
        }
        parts.push_back(cur);
        if (parts.size() < 9) { cout << "Неправильный формат данных врача\n"; return false; }
        string personCompact = parts[0] + "|" + parts[1] + "|" + parts[2] + "|" + parts[3] + "|" + parts[4] + "|" + parts[5] + "|" + parts[6];
        Person p = Person::deserializeCompact(personCompact);
        cout << "Текущие данные врача:\n"; p.showInfo();
        cout << "Введите новые данные:\n"; p.inputInfo();
        string spec; cout << "Введите специализацию: "; getline(cin, spec);
        string exp; cout << "Введите опыт (лет): "; getline(cin, exp);
        it->second.infoCompact = p.serializeCompact() + "|" + spec + "|" + exp;
        it->second.extra = spec;
        it->second.fullName = p.getFullName();
        writeFile(filename);
        cout << "Данные врача обновлены\n";
        return true;
    }

private:
    static void writeString(ofstream& f, const string& s) {
        int len = s.size();
        f.write((char*)&len, sizeof(len));
        f.write(s.data(), len);
    }
    static string readString(ifstream& f) {
        int len;
        f.read((char*)&len, sizeof(len));
        string s(len, '\0');
        f.read(&s[0], len);
        return s;
    }
};

#include "authSys.h"

using namespace std;

string hashPassword(const string& password) {
    try {
        string salt = "medical_system_Ksenon_2025";
        string salted = password + salt;
        hash<string> hasher;
        size_t hashValue = hasher(salted);
        stringstream ss;
        ss << hex << hashValue;
        string result = ss.str();
        return result;
    }
    catch (const exception& e) {
        throw runtime_error("Ошибка хеширования пароля ");
    }
}

AuthSystem::AuthSystem(const string& file) : filename(file) {
    try {
        readFile(filename);
    }
    catch (const exception& e) {
        cout << "Предупреждение: " << e.what() << endl;
    }
}

void AuthSystem::writeString(ofstream& f, const string& s) {
     int len = (int)s.size();
     f.write(reinterpret_cast<const char*>(&len), sizeof(len));
     if (len > 0) {
         f.write(s.data(), len);
     }
     if (f.fail()) {
         throw runtime_error("Ошибка записи строки в файл");
     }
}

string AuthSystem::readString(ifstream& f) {
    int len = 0;
    f.read(reinterpret_cast<char*>(&len), sizeof(len));

    if (f.fail()) {
        throw runtime_error("Ошибка чтения длины строки из файла");
    }

    if (len < 0 || len > 1000000) {
        return "";
    }

    if (len == 0) {
        return "";
    }

    string s(len, '\0');
    f.read(&s[0], len);

    if (f.fail()) {
        throw runtime_error("Ошибка чтения данных строки из файла");
    }

    return s;
}

void AuthSystem::writeFile(const string& file) {
    ofstream f(file, ios::binary | ios::trunc);
    if (!f.is_open()) {
        throw runtime_error("Не удалось открыть файл для записи: " + file);
    }

    int count = users.size();
    f.write(reinterpret_cast<char*>(&count), sizeof(count));

    for (auto& u : users) {
        writeString(f, u.login);
        writeString(f, u.password);
        writeString(f, u.role);
        writeString(f, u.fullName);
    }

    if (f.fail()) {
        throw runtime_error("Ошибка записи данных в файл: " + file);
    }

    f.close();
}

void AuthSystem::readFile(const string& file) {
    ifstream f(file, ios::binary);
    if (!f.is_open()) {
        return;
    }

    int count;
    f.read(reinterpret_cast<char*>(&count), sizeof(count));

    if (f.fail()) {
        throw runtime_error("Ошибка чтения количества пользователей из файла");
    }

    for (int i = 0; i < count; i++) {
        User u;
        u.login = readString(f);
        u.password = readString(f);
        u.role = readString(f);
        u.fullName = readString(f);
        users.push_back(u);
    }

    f.close();
}

bool AuthSystem::addAdmin(const string& login, const string& pass, const string& fullName) {
    try {
        if (login.empty() || pass.empty()) {
            throw invalid_argument("Логин и пароль не могут быть пустыми");
        }

        string hashedPass = hashPassword(pass);

        if (contains<vector<User>>(users, User{ login, "", "", "" })) {
            throw runtime_error("Пользователь с логином '" + login + "' уже существует");
        }

        users.push_back({ login, hashedPass, "admin", fullName });
        writeFile(filename);
        cout << "Администратор успешно добавлен!\n";
        return true;
    }
    catch (const exception& e) {
        cout << "Ошибка добавления администратора: " << e.what() << endl;
        return false;
    }
}

bool AuthSystem::registerPatient(const string& login, const string& pass, const string& fullName) {
    try {
        if (login.empty() || pass.empty() || fullName.empty()) {
            throw invalid_argument("Логин, пароль и ФИО не могут быть пустыми");
        }

        string hashedPass = hashPassword(pass);

        if (contains<vector<User>>(users, User{ login, "", "", "" })) {
            throw runtime_error("Пользователь с логином '" + login + "' уже существует");
        }

        users.push_back({ login, hashedPass, "patient", fullName });
        writeFile(filename);
        cout << "Пациент успешно зарегистрирован!\n";
        return true;
    }
    catch (const exception& e) {
        cout << "Ошибка регистрации пациента: " << e.what() << endl;
        return false;
    }
}

bool AuthSystem::addDoctor(const string& login, const string& pass, const string& fullName) {
    try {
        if (login.empty() || pass.empty() || fullName.empty()) {
            throw invalid_argument("Логин, пароль и ФИО не могут быть пустыми");
        }

        string hashedPass = hashPassword(pass);

        if (contains<vector<User>>(users, User{ login, "", "", "" })) {
            throw runtime_error("Логин с таким именем уже существует");
        }

        users.push_back({ login, hashedPass, "doctor", fullName });
        writeFile(filename);
        cout << "Доктор успешно добавлен!\n";
        return true;
    }
    catch (const exception& e) {
        cout << "Ошибка добавления врача: " << e.what() << endl;
        return false;
    }
}

bool AuthSystem::hasCurrent() {
    return !currentLogin.empty();
}

string AuthSystem::getCurrentLogin() {
    return currentLogin;
}

string AuthSystem::getCurrentFullName() {
    return currentFullName;
}

User* AuthSystem::getUserByLogin(const string& login) {
    for (auto& u : users) {
        if (u.login == login)
            return &u;
    }
    return nullptr;
}

pair<bool, string> AuthSystem::authenticate(const string& login, const string& pass) {
    try {
        if (login.empty() || pass.empty()) {
            throw invalid_argument("Логин и пароль не могут быть пустыми");
        }

        string hashedPass = hashPassword(pass);
        for (auto& u : users) {
            if (u.login == login && u.password == hashedPass) {
                currentLogin = login;
                currentFullName = u.fullName;
                return { true, u.role };
            }
        }
        throw runtime_error("Неверный логин или пароль");
    }
    catch (const exception& e) {
        cout << "Ошибка аутентификации: " << e.what() << endl;
        return { false, "" };
    }
}

pair<bool, string> AuthSystem::authenticate(const User& user) {
    return authenticate(user.login, user.password);
}

void AuthSystem::logout() {
    currentLogin = "";
    currentFullName = "";
}

bool AuthSystem::hasAnyAdmin() {
    for (auto& u : users) {
        if (u.role == "admin") {
            return true;
        }
    }
    return false;
}
#include "authSys.h"

using namespace std;

//функция для хеширования паролей
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
//функция записи строки в бинарный файл
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
//функция чтения строки из бинарного файла
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
//функция сохранения пользователей в бинарный файл
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
//функция выгрузки пользователей из бинарного файла в память
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

//добавляет админа с уникальным логином
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

//регистрирует пациента с уникальным логином
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

//добавляет доктора с уникальным логином
bool AuthSystem::addDoctor(const string& login, const string& pass, const string& fullName) {
    try {
        if (login.empty() || pass.empty() || fullName.empty()) {
            throw invalid_argument("Логин, пароль и ФИО не могут быть пустыми");
        }

        string hashedPass = hashPassword(pass);

        if (contains<vector<User>>(users, User{ login, "", "", "" })) {
            throw runtime_error("Пользователь с логином '" + login + "' уже существует");
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

//проверяет, выполнен ли вход в систему, есть ли пользователь
bool AuthSystem::isLoggedIn() {
    return !currentLogin.empty();
}

//возвращает логин текущего пользователя
string AuthSystem::getCurrentLogin() {
    return currentLogin;
}

//возвращает фио текущего пользователя
string AuthSystem::getCurrentFullName() {
    return currentFullName;
}

//находит пользователя по логику и возвращает указатель на него
User* AuthSystem::getUserByLogin(const string& login) {
    for (auto& u : users) {
        if (u.login == login)
            return &u;
    }
    return nullptr;
}
//возвращает логин пользователя по ФИО
string AuthSystem::getLoginByFullName(const string& fullName) {
    for (auto& u : users) {
        if (u.fullName == fullName) {
            return u.login;
        }
    }
    return "";
}
//обновляет имя в файле
bool AuthSystem::updateUserFullName(const string& login, const string& newFullName) {
    try {
        User* user = getUserByLogin(login);
        if (user) {
            user->fullName = newFullName;
            writeFile(filename);
            return true;
        }
        cout << "Пользователь с логином " << login << " не найден!" << endl;
        return false;
    }
    catch (const exception& e) {
        cout << "Ошибка обновления ФИО: " << e.what() << endl;
        return false;
    }
}
//проверяет соответствие логина и пароля, при успехе возвращает true и роль
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

//перегрузка функции authenticate
pair<bool, string> AuthSystem::authenticate(const User& user) {
    return authenticate(user.login, user.password);
}

//выход из системы, сбрасывает текущего пользователя
void AuthSystem::logout() {
    currentLogin = "";
    currentFullName = "";
}

//проверяет, есть ли в системе админ
bool AuthSystem::hasAnyAdmin() {
    for (auto& u : users) {
        if (u.role == "admin") return true;
    }
    return false;
}
#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
using namespace std;

struct User {
    string login;
    string password;
    string role;
    string fullName;

    bool operator==(const User& other) const {
        return login == other.login;
    }
};

string hashPassword(const string& password);

class AuthSystem {
private:
    vector<User> users;
    string filename;
    string currentLogin;
    string currentFullName;

    void writeString(ofstream& f, const string& s);
    string readString(ifstream& f);
    void writeFile(const string& file);
    void readFile(const string& file);

public:
    AuthSystem(const string& file);

    bool hasCurrent();
    string getCurrentLogin();
    string getCurrentFullName();
    User* getUserByLogin(const string& login);

    bool addAdmin(const string& login, const string& pass, const string& fullName = "Администратор");
    bool registerPatient(const string& login, const string& pass, const string& fullName);
    bool addDoctor(const string& login, const string& pass, const string& fullName);
    bool hasAnyAdmin();

    pair<bool, string> authenticate(const string& login, const string& pass);
    pair<bool, string> authenticate(const User& user);

    void logout();

    template<typename Container, typename T>
    static bool contains(const Container& container, const T& value) {
        return find(container.begin(), container.end(), value) != container.end();
    }
};
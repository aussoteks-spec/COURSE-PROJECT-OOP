#include "authSys.h"

using namespace std;
SetConsoleCP(1251);
SetConsoleOutputCP(1251);
Console::hideCursor();
//ôóíêöèÿ äëÿ õåøèðîâàíèÿ ïàðîëåé
string hashPassword(const string& password) {
    try {
        string salt = "medical_system_Ksenon_2025";
        string salted = password + salt;
        hash<string> hasher;
        int hashValue = hasher(salted);
        stringstream ss;
        ss << hex << hashValue;
        string result = ss.str();
        return result;
    }
    catch (const exception& e) {
        throw runtime_error("Îøèáêà õåøèðîâàíèÿ ïàðîëÿ ");
    }
}

AuthSystem::AuthSystem(const string& file) : filename(file) {
    try {
        readFile(filename);
    }
    catch (const exception& e) {
        cout << "Ïðåäóïðåæäåíèå: " << e.what() << endl;
    }
}
//ôóíêöèÿ çàïèñè ñòðîêè â áèíàðíûé ôàéë
void AuthSystem::writeString(ofstream& f, const string& s) {
     int len = (int)s.size();
     f.write(reinterpret_cast<const char*>(&len), sizeof(len));
     if (len > 0) {
         f.write(s.data(), len);
     }
     if (f.fail()) {
         throw runtime_error("Îøèáêà çàïèñè ñòðîêè â ôàéë");
     }
}
//ôóíêöèÿ ÷òåíèÿ ñòðîêè èç áèíàðíîãî ôàéëà
string AuthSystem::readString(ifstream& f) {
    int len = 0;
    f.read(reinterpret_cast<char*>(&len), sizeof(len));

    if (f.fail()) {
        throw runtime_error("Îøèáêà ÷òåíèÿ äëèíû ñòðîêè èç ôàéëà");
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
        throw runtime_error("Îøèáêà ÷òåíèÿ äàííûõ ñòðîêè èç ôàéëà");
    }

    return s;
}
//ôóíêöèÿ ñîõðàíåíèÿ ïîëüçîâàòåëåé â áèíàðíûé ôàéë
void AuthSystem::writeFile(const string& file) {
    ofstream f(file, ios::binary | ios::trunc);
    if (!f.is_open()) {
        throw runtime_error("Íå óäàëîñü îòêðûòü ôàéë äëÿ çàïèñè: " + file);
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
        throw runtime_error("Îøèáêà çàïèñè äàííûõ â ôàéë: " + file);
    }

    f.close();
}
//ôóíêöèÿ âûãðóçêè ïîëüçîâàòåëåé èç áèíàðíîãî ôàéëà â ïàìÿòü
void AuthSystem::readFile(const string& file) {
    ifstream f(file, ios::binary);
    if (!f.is_open()) {
        return;
    }

    int count;
    f.read(reinterpret_cast<char*>(&count), sizeof(count));

    if (f.fail()) {
        throw runtime_error("Îøèáêà ÷òåíèÿ êîëè÷åñòâà ïîëüçîâàòåëåé èç ôàéëà");
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

//äîáàâëÿåò àäìèíà ñ óíèêàëüíûì ëîãèíîì
bool AuthSystem::addAdmin(const string& login, const string& pass, const string& fullName) {
    try {
        if (login.empty() || pass.empty()) {
            throw invalid_argument("Ëîãèí è ïàðîëü íå ìîãóò áûòü ïóñòûìè");
        }

        string hashedPass = hashPassword(pass);

        if (contains<vector<User>>(users, User{ login, "", "", "" })) {
            throw runtime_error("Ïîëüçîâàòåëü ñ ëîãèíîì '" + login + "' óæå ñóùåñòâóåò");
        }

        users.push_back({ login, hashedPass, "admin", fullName });
        writeFile(filename);
        cout << "Àäìèíèñòðàòîð óñïåøíî äîáàâëåí!\n";
        return true;
    }
    catch (const exception& e) {
        cout << "Îøèáêà äîáàâëåíèÿ àäìèíèñòðàòîðà: " << e.what() << endl;
        return false;
    }
}

//ðåãèñòðèðóåò ïàöèåíòà ñ óíèêàëüíûì ëîãèíîì
bool AuthSystem::registerPatient(const string& login, const string& pass, const string& fullName) {
    try {
        if (login.empty() || pass.empty() || fullName.empty()) {
            throw invalid_argument("Ëîãèí, ïàðîëü è ÔÈÎ íå ìîãóò áûòü ïóñòûìè");
        }

        string hashedPass = hashPassword(pass);

        if (contains<vector<User>>(users, User{ login, "", "", "" })) {
            throw runtime_error("Ïîëüçîâàòåëü ñ ëîãèíîì '" + login + "' óæå ñóùåñòâóåò");
        }

        users.push_back({ login, hashedPass, "patient", fullName });
        writeFile(filename);
        cout << "Ïàöèåíò óñïåøíî çàðåãèñòðèðîâàí!\n";
        return true;
    }
    catch (const exception& e) {
        cout << "Îøèáêà ðåãèñòðàöèè ïàöèåíòà: " << e.what() << endl;
        return false;
    }
}

//äîáàâëÿåò äîêòîðà ñ óíèêàëüíûì ëîãèíîì
bool AuthSystem::addDoctor(const string& login, const string& pass, const string& fullName) {
    try {
        if (login.empty() || pass.empty() || fullName.empty()) {
            throw invalid_argument("Ëîãèí, ïàðîëü è ÔÈÎ íå ìîãóò áûòü ïóñòûìè");
        }

        string hashedPass = hashPassword(pass);

        if (contains<vector<User>>(users, User{ login, "", "", "" })) {
            throw runtime_error("Ïîëüçîâàòåëü ñ ëîãèíîì '" + login + "' óæå ñóùåñòâóåò");
        }

        users.push_back({ login, hashedPass, "doctor", fullName });
        writeFile(filename);
        cout << "Äîêòîð óñïåøíî äîáàâëåí!\n";
        return true;
    }
    catch (const exception& e) {
        cout << "Îøèáêà äîáàâëåíèÿ âðà÷à: " << e.what() << endl;
        return false;
    }
}

//ïðîâåðÿåò, âûïîëíåí ëè âõîä â ñèñòåìó, åñòü ëè ïîëüçîâàòåëü
bool AuthSystem::isLoggedIn() {
    return !currentLogin.empty();
}

//âîçâðàùàåò ëîãèí òåêóùåãî ïîëüçîâàòåëÿ
string AuthSystem::getCurrentLogin() {
    return currentLogin;
}

//âîçâðàùàåò ôèî òåêóùåãî ïîëüçîâàòåëÿ
string AuthSystem::getCurrentFullName() {
    return currentFullName;
}

//íàõîäèò ïîëüçîâàòåëÿ ïî ëîãèêó è âîçâðàùàåò óêàçàòåëü íà íåãî
User* AuthSystem::getUserByLogin(const string& login) {
    for (auto& u : users) {
        if (u.login == login)
            return &u;
    }
    return nullptr;
}
//âîçâðàùàåò ëîãèí ïîëüçîâàòåëÿ ïî ÔÈÎ
string AuthSystem::getLoginByFullName(const string& fullName) {
    for (auto& u : users) {
        if (u.fullName == fullName) {
            return u.login;
        }
    }
    return "";
}
//îáíîâëÿåò èìÿ â ôàéëå
bool AuthSystem::updateUserFullName(const string& login, const string& newFullName) {
    try {
        User* user = getUserByLogin(login);
        if (user) {
            user->fullName = newFullName;
            writeFile(filename);
            return true;
        }
        cout << "Ïîëüçîâàòåëü ñ ëîãèíîì " << login << " íå íàéäåí!" << endl;
        return false;
    }
    catch (const exception& e) {
        cout << "Îøèáêà îáíîâëåíèÿ ÔÈÎ: " << e.what() << endl;
        return false;
    }
}
//óäàëÿåò ïîëüçîâàòåëÿ ïî ëîãèíó
bool AuthSystem::deleteUser(const string& login) {
    try {
        if (login.empty()) {
            throw invalid_argument("Ëîãèí íå ìîæåò áûòü ïóñòûì");
        }

        if (login == currentLogin) {
            throw runtime_error("Íåëüçÿ óäàëèòü òåêóùåãî ïîëüçîâàòåëÿ. Ñíà÷àëà âûéäèòå èç ñèñòåìû.");
        }

        bool found = false;
        for (auto it = users.begin(); it != users.end(); ) {
            if (it->login == login) {
                it = users.erase(it);
                found = true;
                break;
            }
            else {
                ++it;
            }
        }

        if (!found) {
            throw runtime_error("Ïîëüçîâàòåëü ñ ëîãèíîì '" + login + "' íå íàéäåí");
        }

        writeFile(filename);
        return true;
    }
    catch (const exception& e) {
        cout << "Îøèáêà óäàëåíèÿ ïîëüçîâàòåëÿ: " << e.what() << endl;
        return false;
    }
}
//ïðîâåðÿåò ñîîòâåòñòâèå ëîãèíà è ïàðîëÿ, ïðè óñïåõå âîçâðàùàåò true è ðîëü
pair<bool, string> AuthSystem::authenticate(const string& login, const string& pass) {
    try {
        if (login.empty() || pass.empty()) {
            throw invalid_argument("Ëîãèí è ïàðîëü íå ìîãóò áûòü ïóñòûìè");
        }

        string hashedPass = hashPassword(pass);
        for (auto& u : users) {
            if (u.login == login && u.password == hashedPass) {
                currentLogin = login;
                currentFullName = u.fullName;
                return { true, u.role };
            }
        }
        throw runtime_error("Íåâåðíûé ëîãèí èëè ïàðîëü");
    }
    catch (const exception& e) {
        cout << "Îøèáêà àóòåíòèôèêàöèè: " << e.what() << endl;
        return { false, "" };
    }
}

//ïåðåãðóçêà ôóíêöèè authenticate
pair<bool, string> AuthSystem::authenticate(const User& user) {
    return authenticate(user.login, user.password);
}

//âûõîä èç ñèñòåìû, ñáðàñûâàåò òåêóùåãî ïîëüçîâàòåëÿ
void AuthSystem::logout() {
    currentLogin = "";
    currentFullName = "";
}

//ïðîâåðÿåò, åñòü ëè â ñèñòåìå àäìèí
bool AuthSystem::hasAnyAdmin() {
    for (auto& u : users) {
        if (u.role == "admin") return true;
    }
    return false;

}

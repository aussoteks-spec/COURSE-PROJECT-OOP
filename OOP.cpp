#include <iostream>
#include <windows.h>
#include "console.h"
#include "authSys.h"
#include "doctor.h"
#include "visit.h"
#include "menu.h"

using namespace std;

enum USER { DOCTOR, ADMIN, PATIENT, NONE };

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    Console::hideCursor();

    Console console;
    AuthSystem auth("users.dat");
    DoctorSystem dsys("doctors.dat");
    VisitRepository visits("visitHistory.dat");
    RoleMenu roleMenu(console, auth, dsys, visits);
    bool hasAdmin = false;
    cout << "Если хотите создать администратора сейчас — введите 'y', иначе Enter: ";
    string r; getline(cin, r);
    if (!r.empty() && (r[0] == 'y' || r[0] == 'Y')) {
        cout << "Логин админа: "; string alog; getline(cin, alog);
        cout << "Пароль админа: "; string apass; getline(cin, apass);
        auth.addAdmin(alog, apass);
    }

    USER currentRole = NONE;
    string currentLogin;

    while (true) {
        if (!auth.hasCurrent()) {
            vector<string> mainMenu = { "Вход", "Регистрация (пациент)", "Выход" };
            int choice = console.showMenu(mainMenu, "МЕДИЦИНСКАЯ СИСТЕМА КСЕНОН");
            if (choice == -1) { cout << "До свидания!\n"; return 0; }
            switch (choice) {
            case 0: {
                system("cls");
                cout << "Логин: "; string login; getline(cin >> ws, login);
                cout << "Пароль: "; string pass; getline(cin, pass);
                auto res = auth.authenticate(login, pass);
                if (res.first) {
                    currentLogin = login;
                    if (res.second == "doctor") currentRole = DOCTOR;
                    else if (res.second == "admin") currentRole = ADMIN;
                    else if (res.second == "patient") currentRole = PATIENT;
                }
                else currentRole = NONE;
                system("pause");
                break;
            }
            case 1: {
                system("cls");
                cout << "Регистрация пациента\n";
                cout << "Логин: "; string login; getline(cin >> ws, login);
                cout << "Пароль: "; string pass; getline(cin, pass);
                if (auth.registerPatient(login, pass)) {
                    auth.authenticate(login, pass);
                    currentLogin = login;
                    currentRole = PATIENT;
                }
                system("pause");
                break;
            }
            case 2:
                cout << "Выход...\n"; return 0;
            }
        }
        else {
            bool cont = true;
            switch (currentRole) {
            case DOCTOR:
                cont = roleMenu.doctorMenu(auth.getCurrentLogin());
                break;
            case ADMIN:
                cont = roleMenu.adminMenu(auth.getCurrentLogin());
                break;
            case PATIENT:
                cont = roleMenu.patientMenu(auth.getCurrentLogin());
                break;
            default:
                cont = false;
            }
            if (!cont) {
                auth.logout();
            }
        }
    }

    return 0;
}
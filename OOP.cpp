#include <iostream>
#include <windows.h>
#include <stdexcept>
#include <sstream>
#include "console.h"
#include "authSys.h"
#include "doctor.h"
#include "visit.h"
#include "menu.h"
#include "queue.h"
#include "patient.h"
using namespace std;

enum USER { DOCTOR, ADMIN, PATIENT, NONE };

using namespace std;

int safeInputInt(const string& prompt, int min = -1000000, int max = 1000000) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            if (value >= min && value <= max) {
                cin.ignore(1000, '\n');
                return value;
            }
            else {
                cout << "Ошибка: число должно быть от " << min << " до " << max << endl;
            }
        }
        else {
            cin.clear();
            string temp;
            getline(cin, temp);
            cout << "Ошибка: введите корректное число" << endl;
        }
    }
}

string safeInputString(const string& prompt, bool allowEmpty = false) {
    string value;
    while (true) {
        try {
            cout << prompt;
            getline(cin >> ws, value);

            if (!allowEmpty && value.empty()) {
                throw invalid_argument("Поле не может быть пустым");
            }
            break;
        }
        catch (const exception& e) {
            cout << e.what() << endl;
        }
    }
    return value;
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    Console::hideCursor();

    try {
        Console console;
        AuthSystem auth("users.dat");
        DoctorSystem dsys("doctors.dat");
        VisitRepository visits("visitHistory.dat");
        CancelQueue cancelQueue("queueVisit.dat");
        RoleMenu roleMenu(console, auth, dsys, visits, cancelQueue);

        if (!auth.hasAnyAdmin()) {
            cout << "Создание учетной записи администратора:\n";

            string adminLogin = safeInputString("Логин администратора: ");
            string adminPass = safeInputString("Пароль администратора: ");

            if (!auth.addAdmin(adminLogin, adminPass, "Администратор системы")) {
                cout << "Не удалось создать администратора. Завершение работы.\n";
                return 1;
            }
        }

        USER currentRole = NONE;
        string currentLogin;

        while (true) {
            try {
                system("cls");
                if (!auth.hasCurrent()) {
                    vector<string> mainMenu = { "Вход", "Регистрация (пациент)", "Выход" };
                    int choice = console.showMenu(mainMenu, "МЕДИЦИНСКАЯ СИСТЕМА КСЕНОН");

                    if (choice == -1) {
                        cout << "До свидания!\n";
                        return 0;
                    }

                    switch (choice) {
                    case 0: {
                        system("cls");
                        string login = safeInputString("Логин: ");
                        string pass = safeInputString("Пароль: ");

                        auto res = auth.authenticate(login, pass);
                        if (res.first) {
                            currentLogin = login;
                            if (res.second == "doctor") currentRole = DOCTOR;
                            else if (res.second == "admin") currentRole = ADMIN;
                            else if (res.second == "patient") currentRole = PATIENT;

                            string welcomeName = auth.getCurrentFullName();
                            if (res.second == "admin") {
                                welcomeName = "Администратор";
                            }
                            cout << "✓ Успешный вход! Добро пожаловать, " << welcomeName << "!\n";
                        }
                        else {
                            currentRole = NONE;
                        }
                        system("pause");
                        break;
                    }
                    case 1: {
                        system("cls");
                        cout << "Регистрация пациента" << endl;

                        string login = safeInputString("Логин: ");
                        string pass = safeInputString("Пароль: ");
                        string fullName = safeInputString("ФИО: ");

                        if (auth.registerPatient(login, pass, fullName)) {
                            roleMenu.createPatientFromRegistration(login, fullName);

                            User tempUser{ login, pass, "patient", fullName };
                            auth.authenticate(tempUser);

                            currentLogin = login;
                            currentRole = PATIENT;
                            cout << "Успешная регистрация и вход! Добро пожаловать, " << fullName << "!\n";
                        }
                        system("pause");
                        break;
                    }
                    case 2:
                        system("cls");
                        cout << "Будем рады снова видеть Вас в медицинском центре Ксенон\n";
                        return 0;
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
                        cout << "Неизвестная роль пользователя!\n";
                        auth.logout();
                        cont = false;
                        break;
                    }
                    if (!cont) {
                        auth.logout();
                        currentRole = NONE;
                        currentLogin = "";
                    }
                }
            }
            catch (const invalid_argument& e) {
                cout << "Ошибка ввода данных: " << e.what() << endl;
                system("pause");
            }
            catch (const runtime_error& e) {
                cout << "Ошибка выполнения: " << e.what() << endl;
                system("pause");
            }
            catch (const exception& e) {
                cout << "Неизвестная ошибка: " << e.what() << endl;
                system("pause");
            }
        }
    }
    catch (...) {
        cout << "Неизвестная критическая ошибка!\n";
        cout << "Программа будет завершена.\n";
        system("pause");
        return 1;
    }

    return 0;
}
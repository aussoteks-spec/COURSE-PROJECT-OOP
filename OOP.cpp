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
//определяет роли пользователей в систем
enum USER { DOCTOR, ADMIN, PATIENT, NONE };
//безопасный ввод целый чисел
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

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    Console::hideCursor();

    try {
        Console console;// управление интерфейсом консоли
        AuthSystem auth("users.dat");// система аутентификации, сохранение пользователь (логин-парол-роль-фио) в файл users.dat
        DoctorSystem dsys("doctors.dat");// система врачей, сохранение врачей (специализация-логин-фио-расписание) в файл doctors.dat
        VisitRepository visits("visitHistory.dat");// история посещений, сохранение истории пациента в файл visitHistory.dat
        CancelQueue cancelQueue("queueVisit.dat");// очередь отмен, сохранение очереди отмены в файл queueVisit.dat
        RoleMenu roleMenu(console, auth, dsys, visits, cancelQueue);// главное меню
        // Создание первого администратора при первом запуске
        if (!auth.hasAnyAdmin()) {
            string adminLogin, adminPass;
            cout << "Создание учетной записи администратора:\n";

            cout << "Логин администратора: ";
            getline(cin, adminLogin);
            if (adminLogin.empty()) {
                throw invalid_argument("Логин не можeт быть пустым");
            }

            cout << "Пароль администратора: ";
            getline(cin, adminPass);
            if (adminPass.empty()) {
                throw invalid_argument("Пароль не может быть пустым");
            }

            if (!auth.addAdmin(adminLogin, adminPass, "Администратор")) {
                cout << "Не удалось создать администратора. Завершение работы.\n";
                return 1;
            }
        }

        USER currentRole = NONE;
        string currentLogin;

        while (true) {
            try {
                system("cls");
                if (!auth.isLoggedIn()) {
                    vector<string> mainMenu = { "Вход", "Регистрация (пациент)", "Выход" };
                    int choice = console.showMenu(mainMenu, "МЕДИЦИНСКАЯ СИСТЕМА КСЕНОН");

                    if (choice == -1) {
                        system("cls");
                        cout << "Будем рады снова видеть Вас в медицинском центре Ксенон\n";
                        return 0;
                    }

                    switch (choice) {
                    case 0: {
                        system("cls");
                        string login, pass;
                        cout << "Логин : ";
                        getline(cin, login);
                        if (login.empty()) {
                            throw invalid_argument("Логин не можeт быть пустым");
                        }

                        cout << "Пароль : ";
                        getline(cin, pass);
                        if (pass.empty()) {
                            throw invalid_argument("Пароль не может быть пустым");
                        }

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
                            cout << "Успешный вход! Добро пожаловать, " << welcomeName << "!\n";
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
                        roleMenu.createPatientRegistration();
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
        cout << "Программа завершена.\n";
        system("pause");
        return 1;
    }

    return 0;
}
#pragma once
#include "console.h"
#include "authSys.h"
#include "doctor.h"
#include "visit.h"
#include <vector>
#include <iostream>
using namespace std;

class RoleMenu {
    Console& console;
    AuthSystem& auth;
    DoctorSystem& ds;
    VisitRepository& vr;
public:
    RoleMenu(Console& c, AuthSystem& a, DoctorSystem& d, VisitRepository& v) : console(c), auth(a), ds(d), vr(v) {}

    bool doctorMenu(const string& login) {
        vector<string> items = {
            "Показать моё расписание",
            "Отредактировать заключение (добавить запись о приёме)",
            "Поиск пациента по логину",
            "Показать пациентов (последние записи)",
            "Выход"
        };
        int choice = console.showMenu(items, "МЕНЮ ДОКТОРА");
        if (choice == -1 || choice == (int)items.size() - 1) return false;
        system("cls");
        switch (choice) {
        case 0: {
            string spec; DoctorEntry doc;
            if (!ds.findDoctorByLogin(login, spec, doc)) { cout << "Вас нет в системе врачей.\n"; break; }
            auto list = ds.listDoctors(spec);
            int idx = -1;
            for (size_t i = 0; i < list.size(); ++i) if (list[i].first == login) idx = i;
            if (idx == -1) { cout << "Не найден ваш индекс.\n"; break; }
            ds.showSchedule(spec, idx);
            break;
        }
        case 1: {
            cout << "Введите логин пациента: "; string pl; getline(cin >> ws, pl);
            cout << "Введите диагноз/заключение: "; string diag; getline(cin, diag);
            Visit r;
            User pu;
            if (!auth.getUserByLogin(pl, pu)) { cout << "Пациент не найден\n"; break; }
            r.patientLogin = pl;
            r.patientFullName = pu.fullName;
            r.specialization = "(ручное)";
            r.doctorLogin = login;
            r.doctorFullName = auth.getCurrentFullName();
            r.slotString = diag;
            vr.add(r);
            cout << "Заключение добавлено в историю.\n";
            break;
        }
        case 2: {
            cout << "Введите логин пациента: "; string pl; getline(cin >> ws, pl);
            User u;
            if (auth.getUserByLogin(pl, u) && u.role == "patient") {
                cout << "Пациент: " << u.fullName << "\nИнфо: " << u.infoCompact << "\nДоп: " << u.extra << "\n";
            }
            else cout << "Пациент не найден.\n";
            break;
        }
        case 3: {
            auto recs = vr.getAll();
            cout << "Все записи\n";
            for (auto& r : recs) {
                cout << r.slotString << " | " << r.patientFullName << " -> " << r.doctorFullName << " (" << r.specialization << ")\n";
            }
            break;
        }
        }
        system("pause");
        return true;
    }

    bool adminMenu(const string& login) {
        vector<string> items = {
            "Регистрация нового врача",
            "Регистрация нового пациента",
            "Редактировать данные пациента",
            "Редактировать данные врача",
            "Добавить запись пациенту",
            "Отменить запись",
            "Отсортировать врачей по ФИО и специализации",
            "Выход"
        };
        int choice = console.showMenu(items, "МЕНЮ АДМИНИСТРАТОРА");
        if (choice == -1 || choice == (int)items.size() - 1) return false;
        system("cls");
        switch (choice) {
        case 0: {
            cout << "Логин доктора: "; string dlogin; getline(cin >> ws, dlogin);
            cout << "Пароль: "; string pass; getline(cin, pass);
            if (auth.addDoctorByAdmin(login, dlogin, pass)) {
                User u; auth.getUserByLogin(dlogin, u);
                DoctorEntry de; de.login = dlogin; de.fullName = u.fullName; de.schedule = DoctorSystem::generateSchedule();
                ds.addDoctor(u.extra, de);
                cout << "Доктор добавлен в DoctorSystem.\n";
            }
            break;
        }
        case 1: {
            cout << "Логин пациента: "; string pl; getline(cin >> ws, pl);
            cout << "Пароль: "; string pass; getline(cin, pass);
            auth.registerPatient(pl, pass);
            break;
        }
        case 2: {
            cout << "Введите логин пациента для редактирования: "; string pl; getline(cin >> ws, pl);
            auth.editPatientInfoByLogin(pl);
            break;
        }
        case 3: {
            cout << "Введите логин врача для редактирования: "; string dl; getline(cin >> ws, dl);
            auth.editDoctorInfoByLogin(dl);
            break;
        }
        case 4: {
            cout << "Введите логин пациента: "; string pl; getline(cin >> ws, pl);
            User pu;
            if (!auth.getUserByLogin(pl, pu) || pu.role != "patient") { cout << "Пациент не найден\n"; break; }
            ds.showAllSpecs();
            cout << "Введите специализацию: "; string spec; getline(cin >> ws, spec);
            ds.showDoctorsBySpec(spec);
            cout << "Введите номер врача: "; int didx; cin >> didx; cin.ignore(); didx--;
            ds.showSchedule(spec, didx);
            cout << "Введите номер слота: "; int sidx; cin >> sidx; cin.ignore(); sidx--;
            string slotDesc;
            if (!ds.bookAppointment(spec, didx, sidx, slotDesc)) { cout << "Ошибка бронирования\n"; break; }
            Visit vrrec;
            vrrec.patientLogin = pl;
            vrrec.patientFullName = pu.fullName;
            auto list = ds.listDoctors(spec);
            vrrec.doctorLogin = list[didx].first;
            vrrec.doctorFullName = list[didx].second;
            vrrec.specialization = spec;
            vrrec.slotString = slotDesc;
            vr.add(vrrec);
            cout << "Запись добавлена\n";
            break;
        }
        case 5: {
            cout << "Введите логин пациента: "; string pl; getline(cin >> ws, pl);
            auto recs = vr.getByPatient(pl);
            if (recs.empty()) { cout << "Нет записей\n"; break; }
            for (int i = 0; i < recs.size(); ++i)
                cout << i + 1 << ". " << recs[i].slotString << " -> " << recs[i].doctorFullName << "\n";
            cout << "Выберите номер записи для удаления: "; int idx; cin >> idx; cin.ignore(); idx--;
            if (idx < 0 || idx >= (int)recs.size()) { cout << "Неверный индекс\n"; break; }
            vr.remove(recs[idx].patientLogin, recs[idx].doctorLogin, recs[idx].slotString);
            ds.freeSlot(recs[idx].specialization, recs[idx].doctorLogin, recs[idx].slotString);
            cout << "Запись удалена\n";
            break;
        }
        case 6: {
            ds.sortDoctors();
            cout << "Врачи отсортированы\n";
            break;
        }
        }
        system("pause");
        return true;
    }

    bool patientMenu(const string& login) {
        vector<string> items = {
            "Просмотр информации о себе",
            "Просмотр истории посещений",
            "Запись к врачу",
            "Выход"
        };
        int choice = console.showMenu(items, "МЕНЮ ПАЦИЕНТА");
        if (choice == -1 || choice == (int)items.size() - 1) return false;
        system("cls");
        switch (choice) {
        case 0: {
            User u; if (auth.getUserByLogin(login, u)) {
                cout << "Информация:\n";
                if (!u.infoCompact.empty()) {
                    Person p = Person::deserializeCompact(u.infoCompact);
                    p.showInfo();
                }
                cout << "Доп: " << u.extra << "\n";
            }
            else cout << "Ошибка: данные не найдены\n";
            break;
        }
        case 1: {
            auto recs = vr.getByPatient(login);
            if (recs.empty()) { cout << "История пуста\n"; break; }
            for (auto& r : recs) cout << r.slotString << " -> " << r.doctorFullName << " (" << r.specialization << ")\n";
            string out = login + "_visits.txt";
            ofstream of(out);
            of << "Дата/слот\tДоктор\tСпециализация\tПациент\n";
            for (auto& r : recs) of << r.slotString << "\t" << r.doctorFullName << "\t" << r.specialization << "\t" << r.patientFullName << "\n";
            of.close();
            cout << "Экспорт сохранён в " << out << "\n";
            break;
        }
        case 2: {
            ds.showAllSpecs();
            cout << "Введите специализацию: "; string spec; getline(cin >> ws, spec);
            ds.showDoctorsBySpec(spec);
            cout << "Введите номер врача: "; int didx; cin >> didx; cin.ignore(); didx--;
            ds.showSchedule(spec, didx);
            cout << "Введите номер слота: "; int sidx; cin >> sidx; cin.ignore(); sidx--;
            string slotDesc;
            if (!ds.bookAppointment(spec, didx, sidx, slotDesc)) { cout << "Ошибка\n"; break; }
            User pu; auth.getUserByLogin(login, pu);
            Visit rec;
            rec.patientLogin = login;
            rec.patientFullName = pu.fullName;
            auto list = ds.listDoctors(spec);
            rec.doctorLogin = list[didx].first;
            rec.doctorFullName = list[didx].second;
            rec.specialization = spec;
            rec.slotString = slotDesc;
            vr.add(rec);
            cout << "Вы записаны: " << slotDesc << "\n";
            break;
        }
        }
        system("pause");
        return true;
    }
};
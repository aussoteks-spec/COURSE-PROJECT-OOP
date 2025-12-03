#include "menu.h"
#include "doctor.h"
#include "visit.h"
#include "authSys.h"
#include "table.h"
#include <cctype>
#include <sstream>
#include <algorithm>
#include <windows.h>

using namespace std;

enum class PatientType { REGULAR, CHILD, INVALID };

RoleMenu::RoleMenu(Console& c, AuthSystem& a, DoctorSystem& d, VisitRepository& v, CancelQueue& cq)
    : console(c), auth(a), doctors(d), visits(v), cancelQueue(cq), patientsFilename("patients.dat") {
    loadPatients();
}
//функция записи строки в бинарный файл
void RoleMenu::writeString(ofstream& f, const string& s) {
    int len = (int)s.size();
    f.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (len > 0) {
        f.write(s.data(), len);
    }
}
//функция чтения строки из бинарного файла
string RoleMenu::readString(ifstream& f) {
    int len = 0;
    f.read(reinterpret_cast<char*>(&len), sizeof(len));

    if (len < 0 || len > 1000000) {
        return "";
    }

    if (len == 0) {
        return "";
    }

    string s(len, '\0');
    f.read(&s[0], len);
    return s;
}
//функция сохранения пользователей в бинарный файл с учётом их типа
void RoleMenu::savePatients() {
    ofstream f(patientsFilename, ios::binary | ios::trunc);
    if (!f.is_open()) {
        cout << "Ошибка: не удалось открыть файл для сохранения пациентов!" << endl;
        return;
    }

    int count = patients.size();
    f.write(reinterpret_cast<char*>(&count), sizeof(count));

    for (auto& patient : patients) {
        PatientType type = PatientType::REGULAR;
        if (dynamic_cast<Child*>(patient.get())) {
            type = PatientType::CHILD;
        }
        else if (dynamic_cast<Invalid*>(patient.get())) {
            type = PatientType::INVALID;
        }

        f.write(reinterpret_cast<char*>(&type), sizeof(type));

        writeString(f, patient->getInfoString());
        writeString(f, patient->getIllness());

        if (type == PatientType::CHILD) {
            Child* child = dynamic_cast<Child*>(patient.get());
            writeString(f, child->getParentName());
        }
        else if (type == PatientType::INVALID) {
            Invalid* inv = dynamic_cast<Invalid*>(patient.get());
            int group = inv->getDisabilityGroup();
            f.write(reinterpret_cast<char*>(&group), sizeof(group));
        }
    }
    f.close();
}
//функция выгрузки пользователей из бинарного файла в память с учётом их типа
void RoleMenu::loadPatients() {
    ifstream f(patientsFilename, ios::binary);
    if (!f.is_open()) {
        return;
    }

    int count;
    f.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (int i = 0; i < count; i++) {
        PatientType type;
        f.read(reinterpret_cast<char*>(&type), sizeof(type));

        string personStr = readString(f);
        string illness = readString(f);

        Person person = Person::DeserializeFromFile(personStr);
        Person* personPtr = new Person(person);
        shared_ptr<Patient> patient;

        if (type == PatientType::CHILD) {
            string parentName = readString(f);
            patient = make_shared<Child>(personPtr, illness, parentName);
        }
        else if (type == PatientType::INVALID) {
            int group;
            f.read(reinterpret_cast<char*>(&group), sizeof(group));
            patient = make_shared<Invalid>(personPtr, illness, group);
        }
        else {
            patient = make_shared<Patient>(personPtr, illness);
        }

        patients.push_back(patient);
    }
    f.close();
}
//создаёт нового пациента в системе
void RoleMenu::createPatientRegistration() {
    string patientLogin, pass, fullName;
    try {
        cout << "Логин пациента: ";
        getline(cin, patientLogin);
        if (patientLogin.empty()) {
            throw invalid_argument("Логин не можeт быть пустым");
        }

        cout << "Пароль пациента: ";
        getline(cin, pass);
        if (pass.empty()) {
            throw invalid_argument("Пароль не может быть пустым");
        }

        cout << "ФИО пациента: ";
        getline(cin, fullName);
        if (fullName.empty()) {
            throw invalid_argument("ФИО не может быть пустым!");
        }
    }
    catch (const exception& e) {
        throw;
    }
    if (auth.registerPatient(patientLogin, pass, fullName)) {
        try {
            cout << "Тип пациента:\n1. Обычный\n2. Ребёнок\n3. Инвалид\nВыбор: ";
            int type;
            if (!(cin >> type)) {
                cin.clear();
                string temp;
                getline(cin, temp);
                throw invalid_argument("Тип должен быть числом");
            }
            cin.ignore(1000, '\n');

            if (type < 1 || type > 3) {
                throw invalid_argument("Неверный тип пациента");
            }

            Person* person = new Person();
            person->inputInfo();

            string illness;
            cout << "Диагноз: ";
            getline(cin, illness);

            shared_ptr<Patient> patient;
            if (type == 2) {
                string parent;
                cout << "Имя родителя/опекуна: ";
                getline(cin, parent);
                if (parent.empty()) {
                    delete person;
                    throw invalid_argument("Имя родителя/опекуна не может быть пустым");
                }
                patient = make_shared<Child>(person, illness, parent);
            }
            else if (type == 3) {
                int disabilityGroup;
                cout << "Группа инвалидности: ";
                if (!(cin >> disabilityGroup)) {
                    cin.clear();
                    string temp;
                    getline(cin, temp);
                    delete person;
                    throw invalid_argument("Группа инвалидности должна быть числом");
                }
                cin.ignore(1000, '\n');

                if (disabilityGroup < 1 || disabilityGroup > 3) {
                    delete person;
                    throw invalid_argument("Группа инвалидности должна быть от 1 до 3");
                }
                patient = make_shared<Invalid>(person, illness, disabilityGroup);
            }
            else {
                patient = make_shared<Patient>(person, illness);
            }

            patients.push_back(patient);
            savePatients();
            cout << "Пациент успешно добавлен!\n";
        }
        catch (const exception& e) {

            auth.deleteUser(patientLogin);
            cout << "Ошибка при создании пациента! " << e.what() << endl;
        }
    }
    else {
        cout << "Ошибка регистрации!\n";
    }
    system("pause");
}
//поиск пациента по фио
shared_ptr<Patient> RoleMenu::findPatientByFullName(const string& fullName) {
    for (int i = 0; i < patients.size(); i++) {
        if (patients[i]->getFullName() == fullName) {
            return patients[i];
        }
    }
    return nullptr;
}
//вывод на экран списка всех пациентов
void RoleMenu::showAllPatients() {
    cout << "\nВсе пациенты\n";
    if (patients.empty()) {
        cout << "Пациентов нет\n";
        return;
    }
    for (int i = 0; i < patients.size(); i++) {
        cout << i + 1 << ". ";
        patients[i]->show();
    }
}
//сортировка докторов по фио и по специальностям
void RoleMenu::sortDoctors() {
    map<string, vector<DoctorInfo>>& doctorsMap = doctors.getDoctorsMap();
    vector<string> specialties;
    if (doctorsMap.empty()) {
        cout << "Врачей нет" << endl;
        return;
    }

    for (map<string, vector<DoctorInfo>>::iterator it = doctorsMap.begin(); it != doctorsMap.end(); ++it) {
        specialties.push_back(it->first);
    }
    sort(specialties.begin(), specialties.end());

    cout << "\nСписок по специальностям\n";
    for (int i = 0; i < specialties.size(); i++) {
        string spec = specialties[i];
        cout << "\n" << spec << ":\n";
        vector<DoctorInfo>& docs = doctors.getDoctors(spec);
        for (int j = 0; j < docs.size(); j++) {
            cout << "  " << docs[j].fullName << endl;
        }
    }
}
//вывод на экран пациентов по первым буквам фио
void RoleMenu::showPatientsByFirstLetter() {
    map<char, vector<string>> patientsByLetter;

    for (int i = 0; i < patients.size(); i++) {
        string fullName = patients[i]->getFullName();
        if (!fullName.empty()) {
            char firstLetter = toupper(fullName[0]);
            patientsByLetter[firstLetter].push_back(fullName);
        }
    }

    cout << "\nПациенты по первой букве фамилии\n";
    for (map<char, vector<string>>::iterator it = patientsByLetter.begin(); it != patientsByLetter.end(); ++it) {
        cout << "\n" << it->first << ":\n";
        for (int i = 0; i < it->second.size(); i++) {
            cout << "  " << it->second[i] << endl;
        }
    }
}
//запись на приём для пациента
bool RoleMenu::reserveAppointmentForPatient(const string& patientFIO) {
    system("cls");
    doctors.showAllSpecsWithNumbers();

    vector<string> specialties = doctors.getSpecialties();

    if (specialties.empty()) {
        cout << "В системе нет врачей.\n";
        system("pause");
        return false;
    }
    try {
        cout << "\nВведите номер специальности: ";
        int specChoice;
        if (!(cin >> specChoice)) {
            cin.clear();
            string temp;
            getline(cin, temp);
            throw invalid_argument("Номер должен быть числом");
        }
        cin.ignore(1000, '\n');

        if (specChoice < 1 || specChoice > specialties.size()) {
            throw invalid_argument("Неверный номер специальности!");
        }

        string spec = specialties[specChoice - 1];

        system("cls");
        cout << "Врачи по специальности: " << spec << endl;
        doctors.showDoctorsBySpec(spec);

        vector<DoctorInfo>& docsList = doctors.getDoctors(spec);

        if (docsList.empty()) {
            cout << "Нет врачей по этой специальности.\n";
            system("pause");
            return false;
        }

        cout << "Введите номер врача: ";
        int docChoice;
        if (!(cin >> docChoice)) {
            cin.clear();
            string temp;
            getline(cin, temp);
            throw invalid_argument("Номер должен быть числом");
        }
        cin.ignore(1000, '\n');

        if (docChoice < 1 || docChoice > docsList.size()) {
            throw invalid_argument("Неверный номер врача!");
        }
        docChoice--;

        system("cls");
        cout << "Врач: " << docsList[docChoice].fullName << endl;

        cout << "\nВведите номера дней для поиска (1-14, через запятую): ";
        string daysInput;
        getline(cin, daysInput);

        cout << "Введите время для поиска (часы 9-17, через запятую): ";
        string timesInput;
        getline(cin, timesInput);

        vector<int> desiredDays;
        vector<int> desiredHours;

        if (!daysInput.empty()) {
            stringstream daysStream(daysInput);
            string dayStr;
            while (getline(daysStream, dayStr, ',')) {
                dayStr.erase(remove(dayStr.begin(), dayStr.end(), ' '), dayStr.end());
                if (!dayStr.empty()) {
                    int day = stoi(dayStr);
                    if (day >= 1 && day <= 14) {
                        desiredDays.push_back(day);
                    }
                }
            }
        }

        if (!timesInput.empty()) {
            stringstream timesStream(timesInput);
            string hourStr;
            while (getline(timesStream, hourStr, ',')) {
                hourStr.erase(remove(hourStr.begin(), hourStr.end(), ' '), hourStr.end());
                if (!hourStr.empty()) {
                    int hour = stoi(hourStr);
                    if (hour >= 9 && hour <= 17) {
                        desiredHours.push_back(hour);
                    }
                }
            }
        }

        if (desiredDays.empty()) {
            for (int i = 1; i <= 14; i++) desiredDays.push_back(i);
        }
        if (desiredHours.empty()) {
            for (int i = 9; i <= 17; i++) desiredHours.push_back(i);
        }

        sort(desiredDays.begin(), desiredDays.end());
        desiredDays.erase(unique(desiredDays.begin(), desiredDays.end()), desiredDays.end());

        sort(desiredHours.begin(), desiredHours.end());
        desiredHours.erase(unique(desiredHours.begin(), desiredHours.end()), desiredHours.end());

        vector<pair<string, bool>>& schedule = docsList[docChoice].schedule;
        vector<pair<int, string>> filteredFreeSlots;

        cout << "Дни: ";
        for (int d : desiredDays) cout << d << " ";
        cout << endl;
        cout << "Часы: ";
        for (int h : desiredHours) cout << h << " ";
        cout << endl;
        cout << "Всего слотов в расписании: " << schedule.size() << endl;

        for (int i = 0; i < schedule.size(); i++) {
            const auto& slot = schedule[i];
            if (slot.second) {
                string slotStr = slot.first;

                int dayStart = 3;
                int dayEnd = slotStr.find(' ', dayStart);
                if (dayEnd == string::npos) continue;

                int timeStart = dayEnd + 1;
                int colonPos = slotStr.find(':', timeStart);
                if (colonPos == string::npos) continue;

                string dayStr = slotStr.substr(dayStart, dayEnd - dayStart);
                string hourStr = slotStr.substr(timeStart, colonPos - timeStart);

                try {
                    int day = stoi(dayStr);
                    int hour = stoi(hourStr);

                    bool dayMatch = find(desiredDays.begin(), desiredDays.end(), day) != desiredDays.end();
                    bool hourMatch = find(desiredHours.begin(), desiredHours.end(), hour) != desiredHours.end();

                    if (dayMatch && hourMatch) {
                        filteredFreeSlots.push_back({ (int)i, slot.first });
                        cout << "НАЙДЕН СЛОТ: " << slot.first << " -> day=" << day << ", hour=" << hour << endl;
                    }
                }
                catch (const exception& e) {
                    cout << "Ошибка парсинга слота '" << slot.first << "': " << e.what() << endl;
                }
            }
        }

        system("cls");
        cout << "Врач: " << docsList[docChoice].fullName << endl;
        cout << "Критерии поиска:" << endl;
        cout << "Дни: ";
        for (int i = 0; i < desiredDays.size(); i++) {
            cout << desiredDays[i] << (i < desiredDays.size() - 1 ? ", " : "");
        }
        cout << "\nВремя: ";
        for (int i = 0; i < desiredHours.size(); i++) {
            cout << desiredHours[i] << ":00" << (i < desiredHours.size() - 1 ? ", " : "");
        }
        cout << endl;

        if (!filteredFreeSlots.empty()) {
            cout << "\nНАЙДЕННЫЕ СВОБОДНЫЕ СЛОТЫ (" << filteredFreeSlots.size() << "):\n";
            for (int i = 0; i < filteredFreeSlots.size(); i++) {
                cout << i + 1 << ". " << filteredFreeSlots[i].second << endl;
            }

            cout << "\nВведите номер слота для записи (0 для отмены): ";
            int slotChoice;
            if (!(cin >> slotChoice)) {
                cin.clear();
                string temp;
                getline(cin, temp);
                throw invalid_argument("Номер должен быть числом");
            }
            cin.ignore(1000, '\n');

            if (slotChoice == 0) {
                cout << "Запись отменена.\n";
                system("pause");
                return false;
            }

            if (slotChoice < 1 || slotChoice > filteredFreeSlots.size()) {
                throw invalid_argument("Неверный номер слота!");
            }

            int realSlotIndex = filteredFreeSlots[slotChoice - 1].first;
            string selectedSlot = filteredFreeSlots[slotChoice - 1].second;

            int filteredIndex = -1;
            int freeCounter = 0;
            for (int i = 0; i < schedule.size(); i++) {
                if (schedule[i].second) {
                    if (i == (int)realSlotIndex) {
                        filteredIndex = freeCounter;
                        break;
                    }
                    freeCounter++;
                }
            }

            if (filteredIndex != -1) {
                string bookedSlot;
                if (doctors.reserveAppointment(spec, docChoice, filteredIndex, bookedSlot)) {
                    DoctorInfo& doctor = docsList[docChoice];
                    string record = spec + " | " + doctor.fullName + " | " + bookedSlot;

                    visits.addVisit(patientFIO, record, false);

                    auto patient = findPatientByFullName(patientFIO);
                    if (patient) {
                        patient->addVisit(record);
                    }

                    doctors.saveToFile();
                    savePatients();

                    system("cls");
                    cout << "Запись успешно оформлена!\n";
                    cout << "Пациент: " << patientFIO << endl;
                    cout << "Врач: " << doctor.fullName << endl;
                    cout << "Специальность: " << spec << endl;
                    cout << "Время приема: " << bookedSlot << endl;
                    system("pause");
                    return true;
                }
            }
            cout << "Ошибка записи! Слот уже занят или недоступен.\n";
            system("pause");
            return false;
        }
        else {
            cout << "\nНет свободных слотов по указанным критериям.\n";

            vector<string> allFreeSlots;
            for (const auto& slot : schedule) {
                if (slot.second) {
                    allFreeSlots.push_back(slot.first);
                }
            }

            if (!allFreeSlots.empty()) {
                cout << "\nВсе свободные слоты врача (" << allFreeSlots.size() << "):\n";
                for (int i = 0; i < allFreeSlots.size(); i++) {
                    cout << i + 1 << ". " << allFreeSlots[i] << endl;
                }

                cout << "\nХотите записаться на один из этих слотов? (да/нет): ";
                string answer;
                getline(cin, answer);

                transform(answer.begin(), answer.end(), answer.begin(), ::tolower);

                if (answer == "да" || answer == "д" || answer == "yes" || answer == "y") {
                    cout << "\nВведите номер слота для записи (0 для отмены): ";
                    int slotChoice;
                    if (!(cin >> slotChoice)) {
                        cin.clear();
                        string temp;
                        getline(cin, temp);
                        throw invalid_argument("Номер должен быть числом");
                    }
                    cin.ignore(1000, '\n');

                    if (slotChoice == 0) {
                        cout << "Запись отменена.\n";
                        system("pause");
                        return false;
                    }

                    if (slotChoice >= 1 && slotChoice <= allFreeSlots.size()) {
                        int realSlotIndex = -1;
                        int freeCounter = 0;
                        for (int i = 0; i < schedule.size(); i++) {
                            if (schedule[i].second) {
                                freeCounter++;
                                if (freeCounter == slotChoice) {
                                    realSlotIndex = i;
                                    break;
                                }
                            }
                        }

                        if (realSlotIndex != -1) {
                            string bookedSlot;
                            if (doctors.reserveAppointment(spec, docChoice, slotChoice - 1, bookedSlot)) {
                                DoctorInfo& doctor = docsList[docChoice];
                                string record = spec + " | " + doctor.fullName + " | " + bookedSlot;

                                visits.addVisit(patientFIO, record, false);

                                auto patient = findPatientByFullName(patientFIO);
                                if (patient) {
                                    patient->addVisit(record);
                                }

                                doctors.saveToFile();
                                savePatients();

                                system("cls");
                                cout << "Запись успешно оформлена!\n";
                                cout << "Пациент: " << patientFIO << endl;
                                cout << "Врач: " << doctor.fullName << endl;
                                cout << "Специальность: " << spec << endl;
                                cout << "Время приема: " << bookedSlot << endl;
                                system("pause");
                                return true;
                            }
                        }
                    }
                    else {
                        cout << "Неверный номер слота!\n";
                    }
                }
            }
            else {
                cout << "У врача нет свободных слотов.\n";
            }

            system("pause");
            return false;
        }
    }
    catch (const exception& e) {
        throw;
        return false;
    }
}
//меню пациента
bool RoleMenu::patientMenu(const string& login) {
    User* currentUser = auth.getUserByLogin(login);
    string patientFIO = currentUser ? currentUser->fullName : login;

    vector<string> menu = {
        "Просмотреть свои данные",
        "Просмотреть историю посещений",
        "Записаться на прием",
        "Отменить запись",
        "Сохранить историю посещений в файл",
        "Выход"
    };

    int choice = console.showMenu(menu, "Меню пациента: " + patientFIO);

    if (choice == -1 || choice == 5)
        return false;

    switch (choice) {
    case 0: {
        system("cls");
        for (int i = 0; i < patients.size(); i++) {
            if (patients[i]->getFullName() == patientFIO) {
                patients[i]->show();
                system("pause");
                return true;
            }
        }
        cout << "Пациент не найден в базе\n";
        system("pause");
        break;
    }

    case 1: {
        system("cls");
        vector<string> patientVisits = visits.getVisits(patientFIO);
        cout << "\nВаши записи на прием:\n";
        if (patientVisits.empty()) {
            cout << "Записей нет\n";
        }
        else {
            for (int i = 0; i < patientVisits.size(); i++) {
                cout << i + 1 << ". " << patientVisits[i] << endl;
            }
        }
        system("pause");
        break;
    }

    case 2: {
        reserveAppointmentForPatient(patientFIO);
        system("pause");
        break;
    }

    case 3: {
        system("cls");
        vector<string> patientVisits = visits.getVisits(patientFIO);

        if (patientVisits.empty()) {
            cout << "У вас нет запланированных посещений.\n";
            system("pause");
            break;
        }

        cout << "\nВаши записи на прием:\n";
        for (int i = 0; i < patientVisits.size(); i++) {
            cout << i + 1 << ". " << patientVisits[i] << endl;
        }

        cout << "Введите номер записи для отмены: ";
        int cancelChoice;
        if (!(cin >> cancelChoice)) {
            cin.clear();
            string temp;
            getline(cin, temp);
            throw invalid_argument("Номер должен быть числом");
        }
        cin.ignore(1000, '\n');

        if (cancelChoice < 1 || cancelChoice > patientVisits.size()) {
            throw invalid_argument("Неверный номер записи!");
        }

        string visitToCancel = patientVisits[cancelChoice - 1];

        visits.removeVisit(patientFIO, visitToCancel, false);
        cancelQueue.addToQueue(patientFIO, visitToCancel);

        system("pause");
        break;
    }

    case 4: {
        system("cls");
        cout << "Сохранение истории посещений в файл...\n";
        visits.savePersonalVisitFile(patientFIO);
        system("pause");
        break;
    }
    }

    return true;
}
//меню администратора
bool RoleMenu::adminMenu(const string& login) {
    vector<string> menu = {
        "Добавить врача",
        "Добавить пациента",
        "Редактировать данные пациента",
        "Редактировать имя врача",
        "Записать пациента на прием",
        "Отмена записи у пациента",
        "Сортировать врачей по специальностям",
        "Просмотреть очередь отмены записей",
        "Выход"
    };

    int choice = console.showMenu(menu, "Меню администратора");
    if (choice == -1 || choice == 8)
        return false;

    switch (choice) {
    case 0: {
        system("cls");
        string docLogin, pass, fio, spec;
        try {
            cout << "Логин врача: ";
            getline(cin, docLogin);
            if (docLogin.empty()) {
                throw invalid_argument("Логин не можeт быть пустым");
            }
            cout << "Пароль врача: ";
            getline(cin, pass);
            if (pass.empty()) {
                throw invalid_argument("Пароль не можeт быть пустым");
            }
            cout << "ФИО врача: ";
            getline(cin, fio);
            if (fio.empty()) {
                throw invalid_argument("ФИО не может быть пустым!");
            }
            cout << "Специальность: ";
            getline(cin, spec);
            if (spec.empty()) {
                throw invalid_argument("Специальность не может быть пустой!");
            }
            auth.addDoctor(docLogin, pass, fio);
            doctors.addDoctor(spec, docLogin, fio);

        }
        catch (const exception& e) {
            throw;
        }
        system("pause");
        break;
    }

    case 1: {
    system("cls");
    createPatientRegistration();
    break;
    }

    case 2: {
        system("cls");
        showAllPatients();
        try {
            if (!patients.empty()) {
                cout << "Введите ФИО пациента для редактирования: ";
                string fullName;
                getline(cin, fullName);
                if (fullName.empty()) {
                    throw invalid_argument("ФИО не может быть пустым!");
                }
                auto patient = findPatientByFullName(fullName);
                if (patient) {
                    string oldFullName = patient->getFullName();

                    patient->edit();
                    savePatients();

                    string patientLogin = auth.getLoginByFullName(oldFullName);

                    if (!patientLogin.empty()) {
                        auth.updateUserFullName(patientLogin, patient->getFullName());
                    }

                    cout << "Данные пациента обновлены!\n";
                }
                else {
                    cout << "Пациент не найден!\n";
                }
            }
        }
        catch (const exception& e) {
            throw;
        }
        system("pause");
        break;
    }

    case 3: {
        system("cls");
        if (!doctors.showAllSpecs()) {
            system("pause");
            break;
        }
        try {
            cout << "Введите специальность: ";
            string spec;
            getline(cin, spec);
            if (spec.empty()) {
                throw invalid_argument("Специальность не может быть пустой!");
            }
            if (doctors.getDoctors(spec).empty()) {
                cout << "Специальность не найдена!\n";
                system("pause");
                break;
            }

            doctors.showDoctorsBySpec(spec);

            cout << "Введите номер врача: ";
            int idx;
            if (!(cin >> idx)) {
                cin.clear();
                string temp;
                getline(cin, temp);
                throw invalid_argument("Номер должен быть числом");
            }
            cin.ignore(1000, '\n');

            if (idx > 0 && idx <= (int)doctors.getDoctors(spec).size()) {
                string doctorLogin = doctors.getDoctors(spec)[idx - 1].login;
                string oldFullName = doctors.getDoctors(spec)[idx - 1].fullName;

                cout << "Новое ФИО врача: ";
                string newFio;
                getline(cin, newFio);
                if (newFio.empty()) {
                    throw invalid_argument("ФИО не может быть пустым!");
                }

                doctors.getDoctors(spec)[idx - 1].fullName = newFio;
                doctors.saveToFile();

                if (!doctorLogin.empty()) {
                    if (auth.updateUserFullName(doctorLogin, newFio)) {
                    }
                    else {
                        cout << "Не удалось обновить ФИО в системе аутентификации!\n";
                    }
                }
                else {
                    cout << "Не удалось найти логин врача!\n";
                }

                cout << "Имя врача изменено!\n";
            }
            else {
                cout << "Неверный номер!\n";
            }
        }
        catch (const exception& e) {
            throw;
        }
        system("pause");
        break;
    }

    case 4: {
        system("cls");
        showAllPatients();
        try {
            if (!patients.empty()) {
                cout << "Введите номер пациента для записи: ";
                int patientChoice;
                if (!(cin >> patientChoice)) {
                    cin.clear();
                    string temp;
                    getline(cin, temp);
                    throw invalid_argument("Номер должен быть числом");
                }
                cin.ignore(1000, '\n');

                if (patientChoice < 1 || patientChoice > patients.size()) {
                    throw invalid_argument("Неверный номер пациента");
                }

                string patientFIO = patients[patientChoice - 1]->getFullName();
                cout << "Выбран пациент: " << patientFIO << endl;
                cout << "Нажмите Enter для продолжения...";
                cin.get();

                reserveAppointmentForPatient(patientFIO);
                system("pause");
            }
            else {
                cout << "Нет пациентов для записи.\n";
                system("pause");
            }
        }
        catch (const exception& e) {
            throw;
        }
        break;
    }

    case 5: {
        system("cls");
        showAllPatients();
        try {
            if (!patients.empty()) {
                cout << "Введите номер пациента для удаления записи: ";
                int patientChoice;
                if (!(cin >> patientChoice)) {
                    cin.clear();
                    string temp;
                    getline(cin, temp);
                    throw invalid_argument("Номер должен быть числом");
                }
                cin.ignore(1000, '\n');

                if (patientChoice < 1 || patientChoice > patients.size()) {
                    throw invalid_argument("Неверный номер пациента");
                }

                string patientFIO = patients[patientChoice - 1]->getFullName();
                cout << "Выбран пациент: " << patientFIO << endl;

                vector<string> history = visits.getVisits(patientFIO);

                if (!history.empty()) {
                    cout << "\nВсе записи пациента:\n";
                    for (int i = 0; i < history.size(); i++) {
                        cout << i + 1 << ". " << history[i] << endl;
                    }

                    cout << "Введите номер записи для удаления: ";
                    int idx;
                    if (!(cin >> idx)) {
                        cin.clear();
                        string temp;
                        getline(cin, temp);
                        throw invalid_argument("Номер должен быть числом");
                    }

                    if (idx > 0 && idx <= (int)history.size()) {
                        string visitToRemove = history[idx - 1];

                        int firstDelim = visitToRemove.find(" | ");
                        int secondDelim = visitToRemove.find(" | ", firstDelim + 3);

                        if (firstDelim != string::npos && secondDelim != string::npos) {
                            string spec = visitToRemove.substr(0, firstDelim);
                            string doctorName = visitToRemove.substr(firstDelim + 3, secondDelim - firstDelim - 3);
                            string slot = visitToRemove.substr(secondDelim + 3);

                            vector<DoctorInfo>& docs = doctors.getDoctors(spec);
                            string doctorLogin;
                            for (auto& doc : docs) {
                                if (doc.fullName == doctorName) {
                                    doctorLogin = doc.login;
                                    break;
                                }
                            }

                            if (!doctorLogin.empty()) {
                                doctors.freeSlot(spec, doctorLogin, slot);
                                visits.removeVisit(patientFIO, visitToRemove, false);
                                cout << "Запись удалена и слот освобожден!\n";
                            }
                            else {
                                cout << "Не удалось найти врача!\n";
                            }
                        }
                        else {
                            cout << "Ошибка формата записи!\n";
                        }
                    }
                    else {
                        cout << "Неверный номер!\n";
                    }
                }
                else {
                    cout << "У пациента нет записей для удаления.\n";
                }
            }
            else {
                cout << "Нет пациентов для удаления записей.\n";
            }
        }
        catch (const exception& e) {
            throw;
        }
        system("pause");
        break;
    }

    case 6: {
        system("cls");
        sortDoctors();
        system("pause");
        break;
    }

    case 7: {
        system("cls");
        queue<CancelRecord> cancelQueueRecords = cancelQueue.getQueue();

        if (cancelQueueRecords.empty()) {
            cout << "Очередь отмены записей пуста.\n";
            system("pause");
            break;
        }
        try {
            cout << "\nОчередь отмены записей\n";
            queue<CancelRecord> tempQueue = cancelQueueRecords;
            int i = 1;

            while (!tempQueue.empty()) {
                const auto& record = tempQueue.front();
                cout << i << ". Пациент: " << record.patientFIO
                    << " | Запись: " << record.visitRecord << endl;
                tempQueue.pop();
                i++;
            }

            cout << "Подтвердить отмену всех записей и освободить слоты (да/нет)\n";

            string action;
            getline(cin, action);
            if (action.empty()) {
                throw invalid_argument("Строка не может быть пустой");
            }

            for (char& c : action) {
                c = tolower(c);
            }

            if (action == "да" || action == "yes" || action == "д" || action == "y") {
                while (!cancelQueueRecords.empty()) {
                    const auto& record = cancelQueueRecords.front();

                    int firstDelim = record.visitRecord.find(" | ");
                    int secondDelim = record.visitRecord.find(" | ", firstDelim + 3);

                    if (firstDelim != string::npos && secondDelim != string::npos) {
                        string spec = record.visitRecord.substr(0, firstDelim);
                        string doctorName = record.visitRecord.substr(firstDelim + 3, secondDelim - firstDelim - 3);
                        string slot = record.visitRecord.substr(secondDelim + 3);

                        vector<DoctorInfo>& docs = doctors.getDoctors(spec);
                        string doctorLogin;
                        for (auto& doc : docs) {
                            if (doc.fullName == doctorName) {
                                doctorLogin = doc.login;
                                break;
                            }
                        }

                        if (!doctorLogin.empty()) {
                            doctors.freeSlot(spec, doctorLogin, slot);
                        }
                    }
                    cancelQueueRecords.pop();
                }
                cancelQueue.clearQueue();
                cout << "Все слоты освобождены, очередь очищена.\n";
            }
            else if (action == "нет" || action == "no" || action == "н" || action == "n") {
                break;
            }
            else {
                cout << "Неверный ввод!" << endl;
            }
        }
        catch (const exception& e) {
            throw;
        }
        system("pause");
        break;
    }
    }
    return true;
}
//меню врача
bool RoleMenu::doctorMenu(const string& login) {
    string spec;
    DoctorInfo doc;

    if (!doctors.findDoctorByLogin(login, spec, doc)) {
        cout << "Данные врача не найдены\n";
        system("pause");
        return false;
    }

    vector<string> menu = {
        "Просмотреть свое расписание",
        "Изменить диагноз пациента",
        "Найти пациента по ФИО",
        "Просмотреть пациентов по первой букве фамилии",
        "Выход"
    };

    int choice = console.showMenu(menu, "Меню врача: " + doc.fullName);

    if (choice == -1 || choice == 4)
        return false;

    switch (choice) {
    case 0: {
        system("cls");
        vector<DoctorInfo>& list = doctors.getDoctors(spec);

        for (int i = 0; i < list.size(); i++) {
            if (list[i].login == login) {
                TablePrinter::printDoctorScheduleHeader(list[i].fullName, spec);
                TablePrinter::printScheduleTable(list[i].schedule);
                break;
            }
        }
        system("pause");
        break;
    }

    case 1: {
        system("cls");
        showAllPatients();
        try {
            if (!patients.empty()) {
                cout << "Введите ФИО пациента: ";
                string fullName;
                getline(cin, fullName);
                if (fullName.empty()) {
                    throw invalid_argument("ФИО не может быть пустым!");
                }
                auto patient = findPatientByFullName(fullName);
                if (patient) {
                    cout << "Текущий диагноз: " << patient->getIllness() << endl;
                    cout << "Новый диагноз: ";
                    string newIllness;
                    getline(cin, newIllness);
                    if (newIllness.empty()) {
                        throw invalid_argument("Диагноз не может быть пустым!");
                    }
                    patient->setIllness(newIllness);
                    savePatients();
                    cout << "Диагноз изменен!\n";
                }
                else {
                    cout << "Пациент не найден!\n";
                }
            }
        }
        catch (const exception& e) {
            throw;
        }
        system("pause");
        break;
    }

    case 2: {
        system("cls");
        try {
            cout << "Введите ФИО пациента: ";
            string fullName;
            getline(cin, fullName);
            if (fullName.empty()) {
                throw invalid_argument("ФИО не может быть пустым!");
            }
            auto patient = findPatientByFullName(fullName);
            if (patient) {
                patient->show();
            }
            else {
                cout << "Пациент не найден!\n";
            }
        }
        catch (const exception& e) {
            throw;
        }
        system("pause");
        break;
    }

    case 3: {
        system("cls");
        showPatientsByFirstLetter();
        system("pause");
        break;
    }
    }
    return true;
}
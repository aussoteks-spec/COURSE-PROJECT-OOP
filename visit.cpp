#include "visit.h"
#include "doctor.h"
#include <stdexcept>

using namespace std;

VisitRepository::VisitRepository(const string& file) : filename(file) {
    try {
        load();
    }
    catch (const exception& e) {
       cout  << "Предупреждение при загрузке истории посещений: " << e.what() << endl;
    }
}

void VisitRepository::writeString(ofstream& f, const string& s) {
    int len = (int)s.size();
    f.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (len > 0) {
        f.write(s.data(), len);
    }
    if (f.fail()) {
        throw runtime_error("Ошибка записи строки в файл");
    }
}

string VisitRepository::readString(ifstream& f) {
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

void VisitRepository::savePersonalFile(const string& patientFIO, const vector<string>& visits) {
    try {
        if (patientFIO.empty()) {
            throw invalid_argument("ФИО пациента не может быть пустым");
        }

        string userFilename = patientFIO + "_visits.txt";
        ofstream userFile(userFilename);
        if (!userFile.is_open()) {
            throw runtime_error("Не удалось создать файл: " + userFilename);
        } 

        userFile << "История посещений пациента: " << patientFIO << "\n";
        userFile << "------------------------------------\n";

        for (const auto& visit : visits) {
            userFile << visit << "\n";
        }

        userFile << "\nВсего посещений: " << visits.size() << "\n";
        userFile.close();
        cout << "Файл создан: " << userFilename << endl;
    }
    catch (const exception& e) {
        throw runtime_error("Ошибка сохранения файла истории");
    }
}

void VisitRepository::addVisit(const string& patientFIO, const string& record, bool saveToPersonalFile) {
    try {
        if (patientFIO.empty() || record.empty()) {
            throw invalid_argument("ФИО пациента и запись не могут быть пустыми");
        }

        for (auto& p : data) {
            if (p.patientFIO == patientFIO) {
                p.visits.push_back(record);
                save();
                if (saveToPersonalFile) {
                    savePersonalFile(patientFIO, p.visits);
                }
                return;
            }
        }

        PatientVisits pv;
        pv.patientFIO = patientFIO;
        pv.visits.push_back(record);
        data.push_back(pv);
        save();

        if (saveToPersonalFile) {
            savePersonalFile(patientFIO, pv.visits);
        }

        cout << "Запись добавлена для пациента: " << patientFIO << endl;
    }
    catch (const exception& e) {
        cout << "Ошибка добавления записи: " << e.what() << endl;
        throw;
    }
}

void VisitRepository::removeVisit(const string& patientFIO, const string& record, bool saveToPersonalFile) {
    try {
        if (patientFIO.empty() || record.empty()) {
            throw invalid_argument("ФИО пациента и запись не могут быть пустыми");
        }

        for (auto& p : data) {
            if (p.patientFIO == patientFIO) {
                auto it = find(p.visits.begin(), p.visits.end(), record);
                if (it != p.visits.end()) {
                    p.visits.erase(it);
                    save();
                    if (saveToPersonalFile) {
                        savePersonalFile(patientFIO, p.visits);
                    }
                    cout << "Запись удалена: " << record << endl;
                    return;
                }
                else {
                    throw runtime_error("Запись не найдена у пациента");
                }
            }
        }
        throw runtime_error("Пациент не найден");
    }
    catch (const exception& e) {
        cout << "Ошибка удаления записи: " << e.what() << endl;
        throw;
    }
}

vector<string> VisitRepository::getVisits(const string& patientFIO) {
    try {
        if (patientFIO.empty()) {
            throw invalid_argument("ФИО пациента не может быть пустым");
        }

        for (auto& p : data) {
            if (p.patientFIO == patientFIO) {
                vector<string> sortedVisits = p.visits;

                sort(sortedVisits.begin(), sortedVisits.end(),
                    [](const string& a, const string& b) {
                        try {
                            size_t lastPipe = a.find_last_of('|');
                            string slotA = (lastPipe != string::npos) ? a.substr(lastPipe + 2) : a;

                            lastPipe = b.find_last_of('|');
                            string slotB = (lastPipe != string::npos) ? b.substr(lastPipe + 2) : b;

                            return DoctorSystem::slotStringToMinutes(slotA) < DoctorSystem::slotStringToMinutes(slotB);
                        }
                        catch (const exception& e) {
                            cerr << "Ошибка сортировки записей: " << e.what() << endl;
                            return false;
                        }
                    });
                return sortedVisits;
            }
        }
        return {};
    }
    catch (const exception& e) {
        cout << "Ошибка получения записей: " << e.what() << endl;
        return {};
    }
}

void VisitRepository::savePersonalVisitFile(const string& patientFIO) {
    try {
        if (patientFIO.empty()) {
            throw invalid_argument("ФИО пациента не может быть пустым");
        }

        vector<string> patientVisits = getVisits(patientFIO);
        if (!patientVisits.empty()) {
            savePersonalFile(patientFIO, patientVisits);
        }
        else {
            cout << "Нет записей о посещениях для сохранения в файл." << endl;
        }
    }
    catch (const exception& e) {
        cerr << "Ошибка сохранения файла посещений: " << e.what() << endl;
        throw;
    }
}

void VisitRepository::save() {
    ofstream f(filename, ios::binary | ios::trunc);
    if (!f.is_open()) {
        throw runtime_error("Не удалось открыть файл для сохранения: " + filename);
    }

    int count = data.size();
    f.write(reinterpret_cast<char*>(&count), sizeof(count));

    for (auto& p : data) {
        writeString(f, p.patientFIO);

        int vcount = p.visits.size();
        f.write(reinterpret_cast<char*>(&vcount), sizeof(vcount));

        for (auto& v : p.visits) {
            writeString(f, v);
        }
    }

    if (f.fail()) {
        throw runtime_error("Ошибка записи данных в файл");
    }

    f.close();
}

void VisitRepository::load() {
    ifstream f(filename, ios::binary);
    if (!f.is_open()) {
        return;
    }

    int count;
    f.read(reinterpret_cast<char*>(&count), sizeof(count));

    if (f.fail()) {
        throw runtime_error("Ошибка чтения количества записей из файла");
    }

    for (int i = 0; i < count; i++) {
        PatientVisits pv;
        pv.patientFIO = readString(f);

        int vcount;
        f.read(reinterpret_cast<char*>(&vcount), sizeof(vcount));

        for (int j = 0; j < vcount; j++) {
            pv.visits.push_back(readString(f));
        }

        data.push_back(pv);
    }

    f.close();
}
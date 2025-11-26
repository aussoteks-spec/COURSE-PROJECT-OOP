#include "doctor.h"
#include <stdexcept>

using namespace std;

DoctorSystem::DoctorSystem(const string& file) : filename(file) {
    try {
        loadFromFile();
    }
    catch (const exception& e) {
        cout << "Предупреждение при загрузке данных врачей: " << e.what() << endl;
    }
}

void DoctorSystem::writeString(ofstream& f, const string& s) {
    int len = (int)s.size();
    f.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (len > 0) {
        f.write(s.data(), len);
    }
    if (f.fail()) {
        throw runtime_error("Ошибка записи строки в файл");
    }
}

string DoctorSystem::readString(ifstream& f) {
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

vector<pair<string, bool>> DoctorSystem::generateSchedule() {
    vector<pair<string, bool>> schedule;

    for (int d = 1; d <= 14; d++) {
        for (int h = 9; h <= 17; h++) {
            stringstream ss;
            ss << "Day" << setw(2) << setfill('0') << d
                << " " << setw(2) << h << ":00";
            schedule.push_back({ ss.str(), true });
        }
    }
    return schedule;
}

int DoctorSystem::slotStringToMinutes(const string& slot) {
    try {
        if (slot.length() < 12) return 0;

        int day = stoi(slot.substr(3, 2));
        int hour = stoi(slot.substr(9, 2));

        return (day - 1) * 24 * 60 + hour * 60;
    }
    catch (const exception& e) {
        cout << "Ошибка преобразования слота в минуты: " << e.what() << endl;
        return 0;
    }
}

void DoctorSystem::addDoctor(const string& spec, const string& login, const string& fullName) {
    try {
        if (spec.empty() || login.empty() || fullName.empty()) {
            throw invalid_argument("Специальность, логин и ФИО не могут быть пустыми");
        }

        DoctorInfo d;
        d.login = login;
        d.fullName = fullName;
        d.schedule = generateSchedule();
        doctors[spec].push_back(d);
        saveToFile();
        cout << "Врач успешно добавлен!\n";
    }
    catch (const exception& e) {
        cout << "Ошибка добавления врача: " << e.what() << endl;
        throw;
    }
}

bool DoctorSystem::findDoctorByLogin(const string& login, string& outSpec, DoctorInfo& outDoctor) {
    for (auto& pair : doctors) {
        for (auto& doc : pair.second) {
            if (doc.login == login) {
                outSpec = pair.first;
                outDoctor = doc;
                return true;
            }
        }
    }
    return false;
}

void DoctorSystem::showAllSpecs() {
    cout << "\nСпециализации:\n";
    for (auto& pair : doctors) {
        cout << pair.first << " (" << pair.second.size() << ")\n";
    }
}

void DoctorSystem::showAllSpecsWithNumbers() {
    cout << "\nСпециализации\n";
    int counter = 1;
    for (auto& pair : doctors) {
        cout << counter << ". " << pair.first << " (" << pair.second.size() << " врачей)\n";
        counter++;
    }
}

vector<string> DoctorSystem::getSpecialties() {
    vector<string> specialties;
    for (auto& pair : doctors) {
        specialties.push_back(pair.first);
    }
    return specialties;
}

void DoctorSystem::showDoctorsBySpec(const string& spec) {
    try {
        if (!doctors.count(spec)) {
            throw runtime_error("Специальность '" + spec + "' не найдена");
        }

        auto& list = doctors[spec];
        cout << "\nВрачи по специализации: " << spec << endl;

        for (size_t i = 0; i < list.size(); i++) {
            cout << i + 1 << ") " << list[i].fullName << endl;
        }
    }
    catch (const exception& e) {
        cout << "Ошибка: " << e.what() << endl;
    }
}

vector<DoctorInfo>& DoctorSystem::getDoctors(const string& spec) {
    if (!doctors.count(spec)) {
        throw runtime_error("Специальность '" + spec + "' не найдена");
    }
    return doctors[spec];
}

bool DoctorSystem::bookAppointment(const string& spec, int doctorIndex, int slotIndex, string& outSlot) {
    try {
        if (!doctors.count(spec)) {
            throw runtime_error("Специальность не найдена");
        }

        if (doctorIndex < 0 || doctorIndex >= doctors[spec].size()) {
            throw out_of_range("Неверный индекс врача");
        }

        auto& doc = doctors[spec][doctorIndex];
        int freeCounter = 0;
        int realSlotIndex = -1;

        for (size_t i = 0; i < doc.schedule.size(); i++) {
            if (doc.schedule[i].second) {
                if (freeCounter == slotIndex) {
                    realSlotIndex = i;
                    break;
                }
                freeCounter++;
            }
        }

        if (realSlotIndex == -1 ||
            realSlotIndex >= doc.schedule.size() ||
            !doc.schedule[realSlotIndex].second) {
            throw runtime_error("Слот недоступен для записи");
        }

        doc.schedule[realSlotIndex].second = false;
        outSlot = doc.schedule[realSlotIndex].first;
        saveToFile();

        return true;
    }
    catch (const exception& e) {
        cout << "Ошибка записи на прием: " << e.what() << endl;
        return false;
    }
}

bool DoctorSystem::freeSlot(const string& spec, const string& doctorLogin, const string& slot) {
    try {
        if (!doctors.count(spec)) {
            throw runtime_error("Специальность не найдена");
        }

        for (auto& doc : doctors[spec]) {
            if (doc.login == doctorLogin) {
                for (auto& s : doc.schedule) {
                    if (s.first == slot) {
                        s.second = true;
                        saveToFile();
                        return true;
                    }
                }
                throw runtime_error("Слот не найден у врача");
            }
        }
        throw runtime_error("Врач не найден");
    }
    catch (const exception& e) {
        cout << "Ошибка освобождения слота: " << e.what() << endl;
        return false;
    }
}

void DoctorSystem::saveToFile() {
    ofstream f(filename, ios::binary | ios::trunc);
    if (!f.is_open()) {
        throw runtime_error("Не удалось открыть файл для записи: " + filename);
    }

    int specCount = doctors.size();
    f.write(reinterpret_cast<char*>(&specCount), sizeof(specCount));

    for (auto& pair : doctors) {
        writeString(f, pair.first);

        int count = pair.second.size();
        f.write(reinterpret_cast<char*>(&count), sizeof(count));

        for (auto& doc : pair.second) {
            writeString(f, doc.login);
            writeString(f, doc.fullName);

            int slotCount = doc.schedule.size();
            f.write(reinterpret_cast<char*>(&slotCount), sizeof(slotCount));

            for (auto& s : doc.schedule) {
                writeString(f, s.first);
                f.write(reinterpret_cast<char*>(&s.second), sizeof(s.second));
            }
        }
    }

    if (f.fail()) {
        throw runtime_error("Ошибка записи данных в файл");
    }

    f.close();
}

void DoctorSystem::loadFromFile() {
    try {
        ifstream f(filename, ios::binary);
        if (!f.is_open()) {
            return;
        }

        int specCount;
        f.read(reinterpret_cast<char*>(&specCount), sizeof(specCount));

        if (f.fail()) {
            throw runtime_error("Ошибка чтения количества специальностей из файла");
        }

        for (int i = 0; i < specCount; i++) {
            string spec = readString(f);

            int count;
            f.read(reinterpret_cast<char*>(&count), sizeof(count));

            for (int j = 0; j < count; j++) {
                DoctorInfo doc;
                doc.login = readString(f);
                doc.fullName = readString(f);

                int slotCount;
                f.read(reinterpret_cast<char*>(&slotCount), sizeof(slotCount));

                for (int k = 0; k < slotCount; k++) {
                    string slot = readString(f);
                    bool free;
                    f.read(reinterpret_cast<char*>(&free), sizeof(free));
                    doc.schedule.push_back({ slot, free });
                }

                doctors[spec].push_back(doc);
            }
        }

        f.close();
    }
    catch (const exception& e) {
        throw runtime_error("Ошибка загрузки данных врачей");
    }
}
#pragma once
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
using namespace std;

struct DoctorEntry {
    string login; 
    string fullName;
    vector<pair<string, bool>> schedule;
};

class DoctorSystem {
    map<string, vector<DoctorEntry>> doctors;
    string filename;
public:
    DoctorSystem(const string& file = "doctors.dat") : filename(file) {
        loadFromFile();
    }

    static vector<pair<string, bool>> generateSchedule(int days = 14) {
        vector<pair<string, bool>> sch;
        for (int d = 1; d <= days; ++d) {
            for (int h = 9; h <= 16; ++h) {
                stringstream ss;
                ss << "Day" << setw(2) << setfill('0') << d << " " << setw(2) << h << ":00";
                sch.push_back({ ss.str(), true });
            }
        }
        return sch;
    }

    void addDoctor(const string& spec, const DoctorEntry& d) {
        doctors[spec].push_back(d);
        saveToFile();
    }

    void showAllSpecs() const {
        cout << "\nСпециализации\n";
        for (auto& kv : doctors) cout << "- " << kv.first << " (" << kv.second.size() << " врачей)\n";
    }

    void showDoctorsBySpec(const string& spec) const {
        auto it = doctors.find(spec);
        if (it == doctors.end()) { cout << "Нет данной специализации\n"; return; }
        cout << "\nВрачи: " << spec << endl;
        for (size_t i = 0; i < it->second.size(); ++i)
            cout << i + 1 << ". " << it->second[i].fullName << " (login:" << it->second[i].login << ")\n";
    }

    bool findDoctorByLogin(const string& login, string& outSpec, DoctorEntry& outDoc) const {
        for (auto& kv : doctors) {
            for (auto& d : kv.second) {
                if (d.login == login) { outSpec = kv.first; outDoc = d; return true; }
            }
        }
        return false;
    }

    vector<pair<string, string>> listDoctors(const string& spec) const {
        vector<pair<string, string>> res;
        auto it = doctors.find(spec);
        if (it == doctors.end()) return res;
        for (auto& d : it->second) res.push_back({ d.login, d.fullName });
        return res;
    }

    void showSchedule(const string& spec, int idx) const {
        auto it = doctors.find(spec);
        if (it == doctors.end() || idx < 0 || idx >= (int)it->second.size()) { cout << "Врач не найден\n"; return; }
        auto& sch = it->second[idx].schedule;
        cout << "\nРасписание врача " << it->second[idx].fullName << ":\n";
        for (size_t i = 0; i < sch.size(); ++i)
            cout << setw(3) << i + 1 << ". " << sch[i].first << " — " << (sch[i].second ? "свободно" : "занято") << "\n";
    }

    bool bookAppointment(const string& spec, int doctorIndex, int slotIndex, string& outSlotDesc) {
        auto it = doctors.find(spec);
        if (it == doctors.end()) return false;
        if (doctorIndex < 0 || doctorIndex >= (int)it->second.size()) return false;
        auto& sch = it->second[doctorIndex].schedule;
        if (slotIndex < 0 || slotIndex >= (int)sch.size()) return false;
        if (!sch[slotIndex].second) return false;
        sch[slotIndex].second = false;
        outSlotDesc = it->second[doctorIndex].fullName + " (" + spec + ") - " + sch[slotIndex].first;
        saveToFile();
        return true;
    }

    bool freeSlot(const string& spec, const string& doctorLogin, const string& slotString) {
        auto it = doctors.find(spec);
        if (it == doctors.end()) return false;
        for (auto& d : it->second) {
            if (d.login == doctorLogin) {
                for (auto& s : d.schedule) {
                    if (s.first == slotString) {
                        s.second = true;
                        saveToFile();
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void sortDoctors() {
        for (auto& kv : doctors) {
            auto& vec = kv.second;
            sort(vec.begin(), vec.end(), [](const DoctorEntry& a, const DoctorEntry& b) {
                return a.fullName < b.fullName;
                });
        }
        saveToFile();
    }

    void saveToFile() const {
        ofstream f(filename, ios::binary | ios::trunc);
        if (!f.is_open()) return;
        int specCount = doctors.size();
        f.write((char*)&specCount, sizeof(specCount));
        for (auto& kv : doctors) {
            const string& spec = kv.first;
            int len = spec.size();
            f.write((char*)&len, sizeof(len));
            f.write(spec.data(), len);
            int count = kv.second.size();
            f.write((char*)&count, sizeof(count));
            for (auto& d : kv.second) {
                len = d.login.size();
                f.write((char*)&len, sizeof(len));
                f.write(d.login.data(), len);
                len = d.fullName.size();
                f.write((char*)&len, sizeof(len));
                f.write(d.fullName.data(), len);
                int sCount = d.schedule.size();
                f.write((char*)&sCount, sizeof(sCount));
                for (auto& s : d.schedule) {
                    len = s.first.size();
                    f.write((char*)&len, sizeof(len));
                    f.write(s.first.data(), len);
                    f.write((char*)&s.second, sizeof(s.second));
                }
            }
        }
        f.close();
    }

    void loadFromFile() {
        doctors.clear();
        ifstream f(filename, ios::binary);
        if (!f.is_open()) return;
        int specCount;
        f.read((char*)&specCount, sizeof(specCount));
        for (int i = 0; i < specCount; ++i) {
            int len;
            f.read((char*)&len, sizeof(len));
            string spec(len, '\0');
            f.read(&spec[0], len);
            int count;
            f.read((char*)&count, sizeof(count));
            vector<DoctorEntry> list;
            for (int j = 0; j < count; ++j) {
                f.read((char*)&len, sizeof(len));
                string login(len, '\0');
                f.read(&login[0], len);
                f.read((char*)&len, sizeof(len));
                string fullName(len, '\0');
                f.read(&fullName[0], len);
                int sCount;
                f.read((char*)&sCount, sizeof(sCount));
                vector<pair<string, bool>> sch;
                for (int k = 0; k < sCount; ++k) {
                    f.read((char*)&len, sizeof(len));
                    string slot(len, '\0');
                    f.read(&slot[0], len);
                    bool free;
                    f.read((char*)&free, sizeof(free));
                    sch.push_back({ slot, free });
                }
                list.push_back({ login, fullName, sch });
            }
            doctors[spec] = list;
        }
        f.close();
    }
};

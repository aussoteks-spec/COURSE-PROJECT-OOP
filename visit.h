#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>

using namespace std;

struct Visit {
    string patientLogin;
    string patientFullName;
    string specialization;
    string doctorLogin;
    string doctorFullName;
    string slotString;

    friend ostream& operator<<(ostream& os, const Visit& r) {
        os << r.slotString << " | " << r.patientFullName
            << " -> " << r.doctorFullName << " (" << r.specialization << ")";
        return os;
    }
};

class VisitRepository {
private:
    string filename;
    vector<Visit> records;
    static void writeString(ofstream& f, const string& s) {
        int len = s.size();
        f.write((char*)&len, sizeof(len));
        f.write(s.data(), len);
    }
    static string readString(ifstream& f) {
        int len;
        f.read((char*)&len, sizeof(len));
        string s(len, '\0');
        f.read(&s[0], len);
        return s;
    }
public:
    VisitRepository(const string& file = "visitHistory.dat") : filename(file) {
        load();
    }

    void add(const Visit& r) {
        records.push_back(r);
        save();
    }

    bool remove(const string& patientLogin, const string& doctorLogin, const string& slotString) {
        auto it = remove_if(records.begin(), records.end(), [&](const Visit& rec) {
            return rec.patientLogin == patientLogin && rec.doctorLogin == doctorLogin && rec.slotString == slotString;
            });
        if (it == records.end()) return false;
        records.erase(it, records.end());
        save();
        return true;
    }

    vector<Visit> getByPatient(const string& login) const {
        vector<Visit> out;
        for (auto& r : records) if (r.patientLogin == login) out.push_back(r);
        return out;
    }

    vector<Visit> getLastNDays(int days) const {
        vector<Visit> out = records;
        return out;
    }

    vector<Visit> getAll() const {
        return records;
    }

    void save() const {
        ofstream f(filename, ios::binary | ios::trunc);
        if (!f.is_open()) return;
        int n = records.size();
        f.write((char*)&n, sizeof(n));
        for (auto& r : records) {
            writeString(f, r.patientLogin);
            writeString(f, r.patientFullName);
            writeString(f, r.specialization);
            writeString(f, r.doctorLogin);
            writeString(f, r.doctorFullName);
            writeString(f, r.slotString);
        }
        f.close();
    }

    void load() {
        records.clear();
        ifstream f(filename, ios::binary);
        if (!f.is_open()) return;
        int n;
        f.read((char*)&n, sizeof(n));
        for (int i = 0; i < n; ++i) {
            Visit r;
            r.patientLogin = readString(f);
            r.patientFullName = readString(f);
            r.specialization = readString(f);
            r.doctorLogin = readString(f);
            r.doctorFullName = readString(f);
            r.slotString = readString(f);
            records.push_back(r);
        }
        f.close();
    }
};
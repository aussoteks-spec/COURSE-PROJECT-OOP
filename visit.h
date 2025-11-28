#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std;

struct PatientVisits {
    string patientFIO;
    vector<string> visits;
};

class VisitRepository {
private:
    vector<PatientVisits> data;
    string filename;

    void writeString(ofstream& f, const string& s);
    string readString(ifstream& f);
    void savePersonalFile(const string& patientFIO, const vector<string>& visits);

public:
    VisitRepository(const string& file);

    void addVisit(const string& patientFIO, const string& record, bool saveToPersonalFile = false);
    void removeVisit(const string& patientFIO, const string& record, bool saveToPersonalFile = false);
    vector<string> getVisits(const string& patientFIO);
    void savePersonalVisitFile(const string& patientFIO);

    void save();
    void load();
};
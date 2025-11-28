#pragma once
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

struct DoctorInfo {
    string login;
    string fullName;
    vector<pair<string, bool>> schedule;
};

class DoctorSystem {
private:
    map<string, vector<DoctorInfo>> doctors;
    string filename;

    void writeString(ofstream& f, const string& s);
    string readString(ifstream& f);

public:
    DoctorSystem(const string& file);

    static vector<pair<string, bool>> generateSchedule();
    static int slotStringToMinutes(const string& slot);

    void addDoctor(const string& spec, const string& login, const string& fullName);
    bool findDoctorByLogin(const string& login, string& outSpec, DoctorInfo& outDoctor);

    void showAllSpecs();
    void showAllSpecsWithNumbers();
    vector<string> getSpecialties();
    void showDoctorsBySpec(const string& spec);

    vector<DoctorInfo>& getDoctors(const string& spec);

    bool reserveAppointment(const string& spec, int doctorIndex, int slotIndex, string& outSlot);
    bool freeSlot(const string& spec, const string& doctorLogin, const string& slot);

    void saveToFile();
    void loadFromFile();

    map<string, vector<DoctorInfo>>& getDoctorsMap() { return doctors; }
};
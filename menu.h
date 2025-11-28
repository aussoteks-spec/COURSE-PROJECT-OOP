#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <map>
#include <memory>
#include <fstream>
#include "authSys.h"
#include "doctor.h"
#include "visit.h"
#include "console.h"
#include "patient.h"
#include "child.h"
#include "invalid.h"
#include "queue.h"

using namespace std;

class RoleMenu {
private:
    Console& console;
    AuthSystem& auth;
    DoctorSystem& doctors;
    VisitRepository& visits;
    CancelQueue& cancelQueue;
    vector<shared_ptr<Patient>> patients;
    string patientsFilename;

    void writeString(ofstream& f, const string& s);
    string readString(ifstream& f);
    void savePatients();
    void loadPatients();

    shared_ptr<Patient> findPatientByFullName(const string& fullName);
    void showAllPatients();
    void sortDoctors();
    void showPatientsByFirstLetter();
    bool reserveAppointmentForPatient(const string& patientFIO);

public:
    RoleMenu(Console& c, AuthSystem& a, DoctorSystem& d, VisitRepository& v, CancelQueue& cq);

    void createPatientFromRegistration(const string& login, const string& fullName);

    bool adminMenu(const string& login);
    bool doctorMenu(const string& login);
    bool patientMenu(const string& login);
};
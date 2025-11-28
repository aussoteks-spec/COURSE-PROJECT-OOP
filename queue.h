#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

struct CancelRecord {
    string patientFIO;
    string visitRecord;
};

class CancelQueue {
private:
    vector<CancelRecord> queue;
    string filename;

    void writeString(ofstream& f, const string& s);
    string readString(ifstream& f);

public:
    CancelQueue(const string& file);

    void addToQueue(const string& patientFIO, const string& record);
    vector<CancelRecord> getQueue();
    void clearQueue();

    void save();
    void load();
};
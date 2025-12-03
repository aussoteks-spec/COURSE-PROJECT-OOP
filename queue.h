#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <queue> 

using namespace std;

struct CancelRecord {
    string patientFIO;
    string visitRecord;
};

class CancelQueue {
private:
    queue<CancelRecord> queueData;
    string filename;

    void writeString(ofstream& f, const string& s);
    string readString(ifstream& f);

public:
    CancelQueue(const string& file);

    void addToQueue(const string& patientFIO, const string& record);
    queue<CancelRecord> getQueue();
    void clearQueue();
    bool isEmpty() const;
    int size() const;

    void save();
    void load();
};
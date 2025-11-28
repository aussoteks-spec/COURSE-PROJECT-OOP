#pragma once
#include "patient.h"
#include <stdexcept>
#include <iostream>
using namespace std;

class Child : public Patient {
    string parentName;
public:
    Child();
    Child(Person* p, const string& ill, const string& parent);

    string getParentName() const;

    void show() override;
    void edit() override;
};
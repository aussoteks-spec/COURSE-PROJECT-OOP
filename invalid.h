#pragma once
#include "patient.h"
#include <stdexcept>

class Invalid : public Patient {
    int disabilityGroup;
public:
    Invalid();
    Invalid(Person* p, const std::string& ill, int g);

    int getDisabilityGroup() const;

    void show() override;
    void edit() override;
};
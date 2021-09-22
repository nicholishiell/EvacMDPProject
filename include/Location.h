#ifndef Location_H
#define  Location_H

#include <string>

#include "Util.h"
#include "EvacUtil.h"
#include "Population.h"

class Location
{
public:
    Location();

    Location(   const Util::Position& pos, 
                const int cap, 
                const std::string& label, 
                const Population& initialPopulation);
    ~Location();

    void UpdateCapacity(const int newSwingCap, const int newNonCriticalCapcity, const int newCriticalCapacity);

    std::string GetLabel() const;
    Position GetPosition() const;
    int GetTotalCapacity() const;

    IntVector GetPopulationVector() const;
    Population GetPopulation() const;

    void Update(const double deltaTime);

    void UpdatePosition(const Position& pos);

    void RemovePopulation(const IntVector& pop);
    void AddPopulation(const IntVector& pop);

private:

    virtual void typeSpecificUpdate(const double deltaTime) = 0;

    Util::Position position_;

    std::string label_;

    Population population_;

    int totalCapacity_;
    int swingCapacity_;
    int noncriticalCapacity_;
    int criticalCapacity_;
};

#endif
#ifndef EVAC_STATE_H
#define EVAC_STATE_H

#include "State.h"
#include "Util.h"

#include "Site.h"
#include "Vehicle.h"
#include "EvacUtil.h"

class EvacState : public State
{
public:

    EvacState();
    ~EvacState();

    void Update(const ActionSharedPtrVector& action);
    void UpdateDeterministic(const ActionSharedPtrVector& actions);
    void UpdateExogenous();

    void Print() const;  

    double GetCurrentTime() const;
    void SetCurrentTime(const double value);
    double GetDeltaTime() const;
    void SetDeltaTime(const double value);

    VehicleVector GetVehicles() const;
    void AddVehicle(const Vehicle& vehicle);
    Vehicle GetVehicle(const std::string& label) const;

    void AddSite(const Site& site);
    SiteVector GetSites() const;
    Site GetSite(const std::string& label) const;
    
    StateSharedPtr MakeCopy();

private:

    void updateCurrentTimeStamp();

    void popNextTask();

    void removePopulation(std::string locationLabel, IntVector population);
    void addPopulation(std::string locationLabel, IntVector population);

    double deltaTime_; // the amount of time that has passed since the last event
    double currentTime_; 

    SiteVector vectorOfSites_;
    VehicleVector vectorOfVehicles_;
};

#endif
#ifndef VEHICLE_H
#define VEHICLE_H

#include <memory>

#include "Util.h"

#include "Location.h"
#include "Task.h"
#include "EvacUtil.h"
#include "Site.h"

using namespace Util;

class Vehicle : public Location
{
public:
    Vehicle();

    Vehicle(const Site& currentSite,  
            const int cap, 
            const std::string& label,
            const Population& initialPopulation, 
            const VehicleType type,
            const double speed, 
            const double range);
    
    ~Vehicle();

    void AssignTask(const TaskSharedPtr task);
    void CompleteCurrentTask(const double currentTime);

    double GetTaskCompleteTimeStamp() const;
    
    std::string GetCurrentSiteLabel() const;
    TaskSharedPtr GetAssignedTask() const;
    VehicleStatus GetStatus() const;
    double GetRange() const;
    double GetSpeed() const;
    VehicleType GetType() const;  


    void Print() const;

private:

    void typeSpecificUpdate(const double deltaTime);   

    void handleIdleUpdate(const double deltaTime);
    void handleUnloadingUpdate(const double deltaTime);
    void handleLoadingUpdate(const double deltaTime);
    void handleRefuelingUpdate(const double deltaTime);
    void handleRefitUpdate(const double deltaTime);
    void handleFlyingUpdate(const double deltaTime);

    double speed_;
    double range_;

    Util::VehicleStatus status_;

    double taskCompleteTimeStamp_;

    std::string currentSiteLabel_;

    VehicleType type_;

    TaskSharedPtr assignedTask_;
};

#endif
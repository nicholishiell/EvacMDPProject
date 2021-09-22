#ifndef TASK_H
#define  TASK_H

#include <string>
#include <iostream>

#include "Action.h"
#include "Util.h"

#include "EvacUtil.h"
#include "Site.h"

using namespace Util;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class Task : public Action
{
public:

    Task(   const TaskType type,   
            const std::string vehicleLabel, 
            const double assignmentTimeStamp,
            const double reward = 0.);
    ~Task();

    std::string GetVehicleLabel() const;
    double GetAssignmentTimeStamp() const;
    
    TaskType GetType() const;
    void SetType(const TaskType t);

    virtual void Print() = 0;

private:
    // vehicle to which the task is assigned
    std::string vehicleLabel_;

    // used to calculate deltaTime
    double assignmentTimeStamp_;

    // Type of Task
    TaskType type_;

    bool verbose_;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class IdleTask : public Task
{
public:

    IdleTask(   const std::string vehicleLabel, 
                const double assignmentTimeStamp);

    ~IdleTask();

    void Print();

private:

};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class TransportTask : public Task
{
public:

    TransportTask(  const std::string vehicleLabel, 
                    const double assignmentTimeStamp,
                    const double reward,
                    const Site& destination,
                    const IntVector& pop);
    ~TransportTask();

    std::string GetDestinationLabel() const;
    Position GetDestinationPosition() const;
    IntVector GetPopulationVector() const;

    void Print();

private:

    std::string destinationLabel_;
    Position destinationPosition_;

    IntVector pop_;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class ReturnTransportTask : public TransportTask
{
public:

    ReturnTransportTask(const std::string vehicleLabel, 
                        const double assignmentTimeStamp,
                        const double reward,
                        const Site& currentSite,
                        const Site& destination,
                        const IntVector& pop);
    ~ReturnTransportTask();

    std::string GetCurrentSiteLabel() const;
    Position GetCurrentSitePosition() const;

    void Print();

private:

    std::string currentSiteLabel_;
    Position currentSitePosition_;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FerryTask : public Task
{
public:

    FerryTask(  const std::string vehicleLabel, 
                const double assignmentTimeStamp,
                const Site& destination);
    
    FerryTask(  const std::string vehicleLabel, 
                const double assignmentTimeStamp,
                const std::string& destinationLabel,
                const Position& destinationPosition);
    
    ~FerryTask();

    std::string GetDestinationLabel() const;
    Position GetDestinationPosition() const;

    void Print();

private:

    // used in transport and ferry tasks
    std::string destinationLabel_;
    Position destinationPosition_;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class RefitTask : public Task
{
public:

    RefitTask(  const std::string vehicleLabel, 
                const double assignmentTimeStamp,
                const int numberCritical,
                const int numberNonCritical);
    ~RefitTask();

    void Print();

private:

    // used in REFIT task
    int numberCritical_;
    int numberNonCritical_;
};


#endif

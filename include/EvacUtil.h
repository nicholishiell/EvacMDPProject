#ifndef EVAC_UTILITY_H
#define  EVAC_UTILITY_H

#include <memory>
#include <array>
#include <vector>
#include <limits>
#include <string>
#include <cmath>


class Task;
class TransportTask;
class FerryTask;

class EvacMDP;
class EvacState;

class VFA4DPolicy;

class Site;
class Vehicle;

namespace Util
{
    using SiteVector = std::vector<Site>;
    using VehicleVector = std::vector<Vehicle>;
  
    using EvacMDPSharedPtr = std::shared_ptr<EvacMDP>;
    using EvacStateSharedPtr = std::shared_ptr<EvacState>;
    using EvacStateSharedPtrVector = std::vector<EvacStateSharedPtr>;

    using TaskSharedPtr = std::shared_ptr<Task>;
    using TaskSharedPtrVector = std::vector<TaskSharedPtr>; 
    using HistoryOfTasks = std::vector<TaskSharedPtrVector>;

    using Position = std::array<double, 2>;
    using Heading = std::array<double, 2>;
    using PDSV = std::string;

    using VFA4DPolicySharedPtr = std::shared_ptr<VFA4DPolicy>;

    using DoubleVector = std::vector<double>;
    using IntVector = std::vector<int>;    

    const double LARGE_NUMBER = std::numeric_limits<double>::max();
    const int NUMBER_OF_RUNS = 1000;

    enum class VehicleStatus : int {IDLE, REFIT, LOADING, UNLOADING, FLYING, REFUELING, END};

    enum class TaskType : int {IDLE, TRANSPORT, RETURNTRANSPORT, FERRY, REFIT, END};

    enum class SiteType : int {REMOTE, FOB, GOAL, END};

    enum class VehicleType : int {RW, FW, GROUND, END};

    enum class EvacPolicyType : int {USER, PFA, RANDOM, VFA4D};

    std::string IntVector2String(const IntVector vec);
    std::string TaskType2String(const TaskType tt);

    double calculateDist(const Position& p1, const Position& p2);
    Heading calculateHeading(const Position& currentPos, const Position& destinationPos);

    int sumVector(const IntVector& array);
};

#endif
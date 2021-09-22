#include "Task.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ABSTRACT CLASS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Task::Task( const TaskType type,
            const std::string vehicleLabel, 
            const double assignmentTimeStamp, 
            const double reward)
{
    vehicleLabel_ = vehicleLabel;
    assignmentTimeStamp_ = assignmentTimeStamp;
    type_ = type;
 
    SetReward(reward);
    SetBellmanValue(0.);
}
    
Task::~Task()
{

}


std::string 
Task::GetVehicleLabel() const
{
    return vehicleLabel_;
}

double 
Task::GetAssignmentTimeStamp() const
{
    return assignmentTimeStamp_;
}

TaskType 
Task::GetType() const
{
    return type_;
}

void 
Task::SetType(const TaskType t)
{
    type_ = t;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DERIVED CLASSES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ IdleTask ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

IdleTask::IdleTask( const std::string vehicleLabel,
                    const double assignmentTimeStamp)
: Task(TaskType::IDLE, vehicleLabel, assignmentTimeStamp, 0.)
{

}

IdleTask::~IdleTask()
{

}


void 
IdleTask::Print()
{
    std::cout   <<  GetVehicleLabel() << " " 
                << "IDLE" 
                << " @ " << GetAssignmentTimeStamp() << "[hr] "
                << "V: " << GetBellmanValue() << " R: " << GetReward() << std::endl;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TransportTask ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TransportTask::TransportTask(   const std::string vehicleLabel,
                                const double assignmentTimeStamp,
                                const double reward, 
                                const Site& destination,
                                const IntVector& pop)
: Task(TaskType::TRANSPORT, vehicleLabel, assignmentTimeStamp, reward)
{
    destinationLabel_ = destination.GetLabel();
    destinationPosition_ = destination.GetPosition();

    pop_ = pop;   
}

TransportTask::~TransportTask()
{

}

void 
TransportTask::Print()
{
    std::cout   << GetVehicleLabel() << " " 
                << "Transport " << IntVector2String(pop_) <<"to " << destinationLabel_
                << " @ " << GetAssignmentTimeStamp() << "[hr] "
                << "V: " << GetBellmanValue() << " R: " << GetReward() << std::endl;
}

std::string
TransportTask::GetDestinationLabel() const
{
    return destinationLabel_;
}

Position
TransportTask::GetDestinationPosition() const
{
    return destinationPosition_;
}

IntVector 
TransportTask::GetPopulationVector() const
{
    return pop_;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ReturnTransportTask ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ReturnTransportTask::ReturnTransportTask(   const std::string vehicleLabel, 
                                            const double assignmentTimeStamp,
                                            const double reward,
                                            const Site& currentSite,
                                            const Site& destination,
                                            const IntVector& pop) 
: TransportTask(vehicleLabel, assignmentTimeStamp, reward, destination, pop)
{
    currentSiteLabel_ = currentSite.GetLabel();
    currentSitePosition_ = currentSite.GetPosition();

    SetType(TaskType::RETURNTRANSPORT);
}

ReturnTransportTask::~ReturnTransportTask()
{

}

std::string
ReturnTransportTask::GetCurrentSiteLabel() const
{
    return currentSiteLabel_;
}

Position
ReturnTransportTask::GetCurrentSitePosition() const
{
    return currentSitePosition_;
}

void 
ReturnTransportTask::Print()
{
    std::cout   << GetVehicleLabel() << " " 
                << "ReturnTransport " << IntVector2String(GetPopulationVector()) 
                <<"to " << GetDestinationLabel()
                << " @ " << GetAssignmentTimeStamp() << "[hr] "
                << "V: " << GetBellmanValue() << " R: " << GetReward() << std::endl;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FerryTask ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


FerryTask::FerryTask(   const std::string vehicleLabel, 
                        const double assignmentTimeStamp,
                        const Site& destination)
: Task(TaskType::FERRY, vehicleLabel, assignmentTimeStamp, 0.)
{
    destinationLabel_ = destination.GetLabel();
    destinationPosition_ = destination.GetPosition();
}

FerryTask::FerryTask(  const std::string vehicleLabel, 
                    const double assignmentTimeStamp,
                    const std::string& destinationLabel,
                    const Position& destinationPosition)
: Task(TaskType::FERRY, vehicleLabel, assignmentTimeStamp, 0.)
{
    destinationLabel_ = destinationLabel;
    destinationPosition_ = destinationPosition;
}

FerryTask::~FerryTask()
{

}

void 
FerryTask::Print()
{
    std::cout   << GetVehicleLabel() << " " 
                << "FERRY to " << destinationLabel_
                << " @ " << GetAssignmentTimeStamp() << "[hr] " 
                << "V: " << GetBellmanValue() << " R: " << GetReward() << std::endl;
}

std::string
FerryTask::GetDestinationLabel() const
{
    return destinationLabel_;
}

Position
FerryTask::GetDestinationPosition() const
{
    return destinationPosition_;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ RefitTask ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

RefitTask::RefitTask(   const std::string vehicleLabel, 
                        const double assignmentTimeStamp,
                        const int numberCritical,
                        const int numberNonCritical)
: Task(TaskType::REFIT, vehicleLabel, assignmentTimeStamp, 0.)
{   
    numberCritical_ = numberCritical;
    numberNonCritical_ = numberNonCritical;
}

RefitTask::~RefitTask()
{

}

void 
RefitTask::Print()
{
    std::cout<< "V: " << GetBellmanValue() << " R: " << GetReward() << std::endl;
}
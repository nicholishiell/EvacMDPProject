#include <iostream>

#include "Vehicle.h"

Vehicle::Vehicle()
{
    
}

Vehicle::Vehicle(   const Site& currentSite,
                    const int cap, 
                    const std::string& label, 
                    const Population& initialPopulation,
                    const VehicleType type,
                    const double speed, 
                    const double range)
: Location(currentSite.GetPosition(), cap, label, initialPopulation)
{
    speed_ = speed;
    range_ = range;

    currentSiteLabel_ = currentSite.GetLabel();
    
    status_ = Util::VehicleStatus::IDLE;
    type_ = type;

    AssignTask(std::make_shared<IdleTask>(label, 0.));
}

Vehicle::~Vehicle()
{
    
}

void 
Vehicle::AssignTask(const TaskSharedPtr task)
{
    assignedTask_ = task;

    if(assignedTask_->GetType() == TaskType::IDLE)
    {
        taskCompleteTimeStamp_ = LARGE_NUMBER;
        status_ = VehicleStatus::IDLE;
    }
    else if(assignedTask_->GetType() == TaskType::TRANSPORT)
    {                                                                               
        const auto t = std::dynamic_pointer_cast<TransportTask>(assignedTask_);
        
        const auto currentPosition = GetPosition();
        const auto futurePosition = t->GetDestinationPosition();
        const auto distance2Travel = calculateDist(currentPosition, futurePosition);

        taskCompleteTimeStamp_ = (distance2Travel/speed_) + assignedTask_->GetAssignmentTimeStamp();

        status_ = VehicleStatus::FLYING;

        // Add them from the vehicle
        const auto populationVector = t->GetPopulationVector();
        AddPopulation(populationVector);          
    }
    else if(assignedTask_->GetType() == TaskType::RETURNTRANSPORT)
    {                                                                               
        const auto t = std::dynamic_pointer_cast<ReturnTransportTask>(assignedTask_);
        
        const auto currentPosition =GetPosition();
        const auto futurePosition = t->GetDestinationPosition();
        const auto distance2Travel = calculateDist(currentPosition, futurePosition);

        taskCompleteTimeStamp_ = (distance2Travel/speed_) + assignedTask_->GetAssignmentTimeStamp();

        status_ = VehicleStatus::FLYING;

        // Add them from the vehicle
        const auto populationVector = t->GetPopulationVector();   
        AddPopulation(populationVector);  
    }
    else if(assignedTask_->GetType() == TaskType::FERRY)
    {
        const auto t = std::dynamic_pointer_cast<FerryTask>(assignedTask_);      

        const auto currentPosition = GetPosition();
        const auto futurePosition = t->GetDestinationPosition();

        const auto distance2Travel = calculateDist(currentPosition, futurePosition);

        taskCompleteTimeStamp_ = (distance2Travel/speed_) + assignedTask_->GetAssignmentTimeStamp();

        status_ = VehicleStatus::FLYING;
    }
    else if(assignedTask_->GetType() ==TaskType::REFIT)
    {

    }
    else
    {
        // UKNOWN TASK TYPE
    }
}

void
Vehicle::CompleteCurrentTask(const double currentTime)
{
    
    if(assignedTask_->GetType() == TaskType::TRANSPORT)
    {            
        auto t = std::dynamic_pointer_cast<TransportTask>(assignedTask_);
        
        // Remove them from the vehicle
        RemovePopulation(GetPopulationVector());

        currentSiteLabel_ =  t->GetDestinationLabel();
        UpdatePosition(t->GetDestinationPosition());      

        AssignTask(std::make_shared<IdleTask>(GetLabel(), currentTime));
    }
    else if(assignedTask_->GetType() == TaskType::RETURNTRANSPORT)
    {
        auto t = std::dynamic_pointer_cast<ReturnTransportTask>(assignedTask_);   
       
        // Remove them from the vehicle
        RemovePopulation(GetPopulationVector());

        currentSiteLabel_ =  t->GetDestinationLabel();
        UpdatePosition(t->GetDestinationPosition());      

        AssignTask(std::make_shared<FerryTask>( GetLabel(), 
                                                GetTaskCompleteTimeStamp(), 
                                                t->GetCurrentSiteLabel(),
                                                t->GetCurrentSitePosition()));
    }
    else if(assignedTask_->GetType() == TaskType::FERRY)
    {
        auto t = std::dynamic_pointer_cast<FerryTask>(assignedTask_);
        
        currentSiteLabel_ =  t->GetDestinationLabel();
        UpdatePosition(t->GetDestinationPosition());      

        AssignTask(std::make_shared<IdleTask>(GetLabel(), currentTime));
    }
    else if(assignedTask_->GetType() == TaskType::REFIT)
    {
        // Not yet implemented.
        
        AssignTask(std::make_shared<IdleTask>(GetLabel(), currentTime));
    }
    else
    {
        // Unknow status!
    }
}

void 
Vehicle::Print() const
{
    std::cout << "~~~~~~~~~~ VEHICLE ~~~~~~~~~~~~~" << std::endl;
    std::cout << "Name: " << GetLabel() << std::endl;
    std::cout << "Position: " << currentSiteLabel_ << " " <<  GetPosition()[0] << "," << GetPosition()[1] << std::endl;
    std::cout << "Capacity: " << GetTotalCapacity() << std::endl;
    std::cout << "Speed: " << speed_ << " [km/h]" << std::endl;
    std::cout << "Range: " << range_ << " [km]" << std::endl;
    std::cout << std::endl;
    assignedTask_->Print();
}

TaskSharedPtr 
Vehicle::GetAssignedTask() const
{
    return assignedTask_;
}

double 
Vehicle::GetRange() const
{
    return range_;
}

double 
Vehicle::GetSpeed() const
{
    return speed_;
}

VehicleStatus 
Vehicle::GetStatus() const
{
    return status_;
}

std::string
Vehicle::GetCurrentSiteLabel() const
{
    return currentSiteLabel_;
}

double 
Vehicle::GetTaskCompleteTimeStamp() const
{
    return taskCompleteTimeStamp_;
}

VehicleType 
Vehicle::GetType() const
{
    return type_;
}

void
Vehicle::typeSpecificUpdate(const double deltaTime)
{
    if(status_ == VehicleStatus::FLYING)
    {
        handleFlyingUpdate(deltaTime);
    }
    else if(status_ == VehicleStatus::REFIT)
    {
        handleRefitUpdate(deltaTime);
    }
    else if(status_ == VehicleStatus::REFUELING)
    {
        handleRefuelingUpdate(deltaTime);
    }
    else if(status_ == VehicleStatus::LOADING)
    {
        handleLoadingUpdate(deltaTime);
    }
    else if(status_ == VehicleStatus::UNLOADING)
    {
        handleUnloadingUpdate(deltaTime);
    }
    else
    {
        handleIdleUpdate(deltaTime);
    }
}

void 
Vehicle::handleIdleUpdate(const double deltaTime)
{

}

void 
Vehicle::handleUnloadingUpdate(const double deltaTime)
{

}

void 
Vehicle::handleLoadingUpdate(const double deltaTime)
{

}

void 
Vehicle::handleRefuelingUpdate(const double deltaTime)
{

}

void
Vehicle::handleRefitUpdate(const double deltaTime)
{

}

void 
Vehicle::handleFlyingUpdate(const double deltaTime)
{
    int c = 0;
    Position newPos;
    if(assignedTask_->GetType() == TaskType::RETURNTRANSPORT)
    {
        const auto t = std::dynamic_pointer_cast<ReturnTransportTask>(assignedTask_);

        auto distRemaining = calculateDist(t->GetDestinationPosition(), this->GetPosition());
        auto heading = calculateHeading(this->GetPosition(),t->GetDestinationPosition());
        
        auto distTravelled  = speed_*deltaTime;       

        newPos[0] = GetPosition()[0] + distTravelled*heading[0];
        newPos[1] = GetPosition()[1] + distTravelled*heading[1];
    }
    else if(assignedTask_->GetType() == TaskType::FERRY)
    {
        const auto t = std::dynamic_pointer_cast<FerryTask>(assignedTask_);
        auto distRemaining = calculateDist(t->GetDestinationPosition(), this->GetPosition());
        auto heading = calculateHeading(this->GetPosition(),t->GetDestinationPosition());
        
        auto distTravelled  = speed_*deltaTime;       

        newPos[0] = GetPosition()[0] + distTravelled*heading[0];
        newPos[1] = GetPosition()[1] + distTravelled*heading[1];
    }

    UpdatePosition(newPos);   

}
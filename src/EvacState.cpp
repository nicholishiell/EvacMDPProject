#include <iostream>

#include "EvacState.h"

EvacState::EvacState()
{
    deltaTime_ = 0.;
    currentTime_ = 0.;
}

EvacState::~EvacState()
{
    
}

void 
EvacState::Print() const
{
    std::cout << "~~~~~~~~~~~~~~~~ CURRENT STATE ~~~~~~~~~~~~~~~~" << std::endl;
    std::cout << "Time: " << currentTime_ << std::endl;

    for(const auto& b : vectorOfSites_)
    {
        b.Print();
    }

    for(const auto& v : vectorOfVehicles_)
    {
        v.Print();
    }

    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
}

StateSharedPtr 
EvacState::MakeCopy()
{
    EvacStateSharedPtr copyOfState = std::make_shared<EvacState>();

    for(const auto& v : vectorOfVehicles_)
    {
        copyOfState->AddVehicle(v);
    }

    for(const auto& s : vectorOfSites_)
    {
        copyOfState->AddSite(s);
    }

    copyOfState->SetCurrentTime(currentTime_);
    copyOfState->SetDeltaTime(deltaTime_);

    return copyOfState;
}

void 
EvacState::Update(const ActionSharedPtrVector& actions)
{
    UpdateDeterministic(actions);
    UpdateExogenous();
}

void 
EvacState::UpdateDeterministic(const ActionSharedPtrVector& actions)
{
    for(const auto& action : actions)
    {
        const auto task = std::dynamic_pointer_cast<Task>(action);
        
        for(auto& v : vectorOfVehicles_)
        {
            if(v.GetLabel() == task->GetVehicleLabel())
            {
                // TODO: Fix duplicate code
                if(task->GetType() == TaskType::TRANSPORT)
                {                                                                               
                    const auto t = std::dynamic_pointer_cast<TransportTask>(task);
                    
                    // Transfer individuals from the site
                    const auto populationVector = t->GetPopulationVector();
                    removePopulation(v.GetCurrentSiteLabel(), populationVector);
                }
                else if(task->GetType() == TaskType::RETURNTRANSPORT)
                { 
                    const auto t = std::dynamic_pointer_cast<TransportTask>(task);
                    
                    // Transfer individuals from the site
                    const auto populationVector = t->GetPopulationVector();
                    removePopulation(v.GetCurrentSiteLabel(), populationVector);
                }
                else
                {
                    // nothing to be done for other task types
                }

                // Assign the task to the vehicle
                v.AssignTask(task);
            }
        }
    }
}

void 
EvacState::UpdateExogenous()
{
    popNextTask();
    
    // Update all the sites
    for(auto& site : vectorOfSites_)
    {
        site.Update(deltaTime_);
    }
  
    // Update all the vehicles
    for(auto& vehicle : vectorOfVehicles_)
    {
        vehicle.Update(deltaTime_);   
    }   

    updateCurrentTimeStamp();
}

void 
EvacState::AddSite(const Site& site)
{
   vectorOfSites_.emplace_back(site);
}

void 
EvacState::AddVehicle(const Vehicle& vehicle)
{
    vectorOfVehicles_.emplace_back(vehicle);
}

double 
EvacState::GetCurrentTime() const
{
    return currentTime_;
}

void 
EvacState::SetCurrentTime(const double value)
{
    currentTime_ = value;
}

double 
EvacState::GetDeltaTime() const
{
    return deltaTime_;
}

void
EvacState::SetDeltaTime(const double value)
{
    deltaTime_ = value;
}


VehicleVector
EvacState::GetVehicles() const
{
    return vectorOfVehicles_;
}

Vehicle 
EvacState::GetVehicle(const std::string& label) const
{
    Vehicle selectedVehicle;

    for(const auto& v : vectorOfVehicles_)
    {
        if(v.GetLabel() == label)
        {
            selectedVehicle = v;
        }
    }

    return selectedVehicle;
}

SiteVector
EvacState::GetSites() const
{
    return vectorOfSites_;
}

Site 
EvacState::GetSite(const std::string& label) const
{
    Site selectedSite;

    for(const auto& site : vectorOfSites_)
    {
        if(site.GetLabel() == label)
        {
            selectedSite = site;
        }
    }

    return selectedSite;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void 
EvacState::popNextTask()
{
    // Find the next task which will be completed
    int indexOfNextFinishedVehicle = -1;
    double minTimeStamp = LARGE_NUMBER;
   
    for(uint iVehicle = 0; iVehicle < vectorOfVehicles_.size(); iVehicle++)
    {
        auto& v = vectorOfVehicles_[iVehicle];
        if(v.GetTaskCompleteTimeStamp() < minTimeStamp)
        {
            minTimeStamp = v.GetTaskCompleteTimeStamp();
            indexOfNextFinishedVehicle = iVehicle;
        }
    }

    if(indexOfNextFinishedVehicle != -1)
    {
        auto& v = vectorOfVehicles_[indexOfNextFinishedVehicle];
        
        const auto& task = v.GetAssignedTask();

        // TODO: Fix duplicate code
        if(task->GetType() == TaskType::TRANSPORT)
        {                                                                               
            const auto t = std::dynamic_pointer_cast<TransportTask>(task);
            
            // Transfer individuals from the site
            const auto populationVector = t->GetPopulationVector();
            addPopulation(t->GetDestinationLabel(), populationVector);
        }
        else if(task->GetType() == TaskType::RETURNTRANSPORT)
        { 
            const auto t = std::dynamic_pointer_cast<TransportTask>(task);
            
            // Transfer individuals from the site
            const auto populationVector = t->GetPopulationVector();
            addPopulation(t->GetDestinationLabel(), populationVector);
        }
        else
        {
            // nothing to be done for other task types
        }

        v.CompleteCurrentTask(currentTime_);
    }
    else
    {
        minTimeStamp = currentTime_;
    }

    // calculate length of time between current time and task completions
    deltaTime_ = minTimeStamp - currentTime_;
}

void 
EvacState::updateCurrentTimeStamp()
{
    currentTime_ += deltaTime_;
}

void 
EvacState::removePopulation(std::string locationLabel, IntVector population)
{
    for(auto& site : vectorOfSites_)
    {
        if(site.GetLabel() == locationLabel)
        {
            site.RemovePopulation(population);
            return;
        }
    }
   
    for(auto& vehicle : vectorOfVehicles_)
    {
        if(vehicle.GetLabel() == locationLabel)
        {
            vehicle.RemovePopulation(population);
            return;
        }
    }
}
    
void 
EvacState::addPopulation(std::string locationLabel, IntVector population)
{
    for(auto& site : vectorOfSites_)
    {
        if(site.GetLabel() == locationLabel)
        {
            site.AddPopulation(population);
            return;
        }
    }
   
    for(auto& vehicle : vectorOfVehicles_)
    {
        if(vehicle.GetLabel() == locationLabel)
        {
            vehicle.AddPopulation(population);
            return;
        }
    }
}



#include "EvacMDP.h"

#include "EvacState.h"

EvacMDP::EvacMDP()
{

}

EvacMDP::~EvacMDP()
{

}

void 
EvacMDP::Print() const
{
    //std::cout << "Current Reward: " << totalReward_ << std::endl;
}

bool 
EvacMDP::IsEndState() const
{
    int individualsRemaining = 0;

    for(const auto& s : CurrentState()->GetSites())
    {
        if(!s.IsGoal())
        {
            individualsRemaining += sumVector(s.GetPopulationVector());
        }
    }

    return individualsRemaining == 0;
}

void 
EvacMDP::Return2InitialState()
{
    //const auto& currentState = std::make_shared<EvacState>(*InitialState());
    const auto& currentState = InitialState()->MakeCopy();
    SetCurrentState(currentState);
    
    SetTotalReward(0.);
}

void 
EvacMDP::DecisionUpdate(const ActionSharedPtrVector& actions)
{
    GetCurrentState()->UpdateDeterministic(actions);
}

void 
EvacMDP::ExogenousUpdate()
{
    GetCurrentState()->UpdateExogenous();
}

void 
EvacMDP::UpdateTotalReward()
{
    double reward = 0.;

    for(const auto& s : CurrentState()->GetSites())
    {
        if(s.IsGoal())
        {
            reward += sumVector(s.GetPopulationVector());
        }
    }

    SetTotalReward(reward);
}

ActionSharedPtrVector 
EvacMDP::GetLegalActions() const
{
    ActionSharedPtrVector allLegalActions;
   
    for(const auto& vehicle : CurrentState()->GetVehicles())
    {
        // skip all the vehicles that are currently performing a task
        if(vehicle.GetStatus() != VehicleStatus::IDLE) continue;

        auto tasks  = getLegalTasks(vehicle);

        allLegalActions.insert(allLegalActions.end(), tasks.begin(), tasks.end());
    }  

    return allLegalActions;
}

EvacStateSharedPtr 
EvacMDP::CurrentState() const
{
    return std::dynamic_pointer_cast<EvacState>(GetCurrentState());
}

EvacStateSharedPtr 
EvacMDP::InitialState() const
{
    return std::dynamic_pointer_cast<EvacState>(GetInitialState());
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Private Member Functions:
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TaskSharedPtrVector 
EvacMDP::getLegalTasks(const Vehicle& vehicle) const
{
    TaskSharedPtrVector legalTasks;

    // This for-loop is just a clever way to loop over the 'enum class'
    for(int taskType = 0; taskType != static_cast<int>(TaskType::END); taskType++)
    {
        if(taskType == static_cast<int>(TaskType::IDLE))
        {
            handleLegalTask_Idle(vehicle, legalTasks);        
        }
        else if(taskType == static_cast<int>(TaskType::TRANSPORT))
        {
            handleLegalTask_Transport(vehicle, legalTasks);        
        }
        else if(taskType == static_cast<int>(TaskType::FERRY))
        {
            handleLegalTask_Ferry(vehicle, legalTasks);        
        }
        else if(taskType == static_cast<int>(TaskType::REFIT))
        {
            handleLegalTask_Refit(vehicle, legalTasks);        
        }
        else
        {
            // This should never happen!
        }
    }

    return legalTasks;
}

// TODO: This function needs to be fixed by removing duplicate code!
double 
EvacMDP::calculateExpectedReward(   const StateSharedPtr state, 
                                    const ActionSharedPtr decision)
{
    double reward = 0.;
                
    const auto& task = std::dynamic_pointer_cast<Task>(decision);

    if(task->GetType() == TaskType::TRANSPORT)
    {
        const auto t = std::dynamic_pointer_cast<TransportTask>(task);
        auto destination = CurrentState()->GetSite(t->GetDestinationLabel());

        if(destination.IsGoal())
        {
            reward += sumVector(t->GetPopulationVector());
        }
    
    }
    else if(task->GetType() == TaskType::RETURNTRANSPORT)
    {
        const auto t = std::dynamic_pointer_cast<ReturnTransportTask>(task);
        auto destination =CurrentState()->GetSite(t->GetDestinationLabel());
    
        if(destination.IsGoal())
        {
            reward += sumVector(t->GetPopulationVector());
        }
    }
    else
    {
        // this shouldn't happen
    }

    return reward;
}

void 
EvacMDP::handleLegalTask_Idle(  const Vehicle& vehicle, 
                                TaskSharedPtrVector& legalTasks) const
{
    TaskSharedPtr task = std::make_shared<IdleTask>(vehicle.GetLabel(), 
                                                    CurrentState()->GetCurrentTime());

    legalTasks.emplace_back(std::move(task));
}

void 
EvacMDP::handleLegalTask_Transport(const Vehicle& vehicle, TaskSharedPtrVector& legalTasks) const
{
    const auto sitesInRange = getAllSitesInRange(vehicle);  
    const auto legalPayloads = getAllLegalPayloads(vehicle);
    
    for(const auto& site : sitesInRange)
    {
        for(const auto& payload : legalPayloads)
        {
            TaskSharedPtr task1 = std::make_shared<TransportTask>(  vehicle.GetLabel(), 
                                                                   CurrentState()->GetCurrentTime(),  
                                                                    sumVector(payload),
                                                                    site,
                                                                    payload);
            
            TaskSharedPtr task2 = std::make_shared<ReturnTransportTask>(vehicle.GetLabel(), 
                                                                        CurrentState()->GetCurrentTime(),  
                                                                        sumVector(payload),
                                                                        CurrentState()->GetSite(vehicle.GetCurrentSiteLabel()),
                                                                        site,
                                                                        payload);
            
            legalTasks.emplace_back(std::move(task1));
            legalTasks.emplace_back(std::move(task2));
        }
    }
}

void 
EvacMDP::handleLegalTask_Ferry( const Vehicle& vehicle, 
                                TaskSharedPtrVector& legalTasks) const
{
    const auto sitesInRange = getAllSitesInRange(vehicle);

    for(const auto& site : sitesInRange)
    {
        TaskSharedPtr task = std::make_shared<FerryTask>(   vehicle.GetLabel(), 
                                                            CurrentState()->GetCurrentTime(),  
                                                            site);                                                            
        legalTasks.emplace_back(std::move(task));  
    }
}

void 
EvacMDP::handleLegalTask_Refit( const Vehicle& vehicle, 
                                TaskSharedPtrVector& legalTasks) const
{
    // Not implemented yet
}


int
getCapacityRequired(const IntVector& payload, 
                    const IntVector& size)
{
	int capRequired = 0;
	
    for(uint i = 0; i < payload.size(); i++)
	{
		capRequired += size[i]*payload[i];
	}

    return capRequired;
}


bool
isLegal(const IntVector& pop, 
        const IntVector& size, 
        const IntVector& payload, 
        const int capLimit)
{
	return getCapacityRequired(payload, size) <= capLimit;
}

void
recurvSolve(std::vector<IntVector>& legalPayloads, 
		    IntVector payload,
		    const IntVector& pop,
		    const IntVector& size,
		    const int capLimit,
		    const int curTriageCat)
{
	if(curTriageCat == pop.size())
	{
    	if(isLegal(pop, size, payload, capLimit))
		{
			legalPayloads.emplace_back(payload);
			return;
		}	
	}
    else if(!isLegal(pop, size, payload, capLimit))
	{
        return;
	}
	else
	{
		for(int i = 0; i <= pop[curTriageCat]; i++)
		{	

			payload[curTriageCat] = i;
			recurvSolve(	legalPayloads, 
					payload,
					pop,
					size,
					capLimit,
					curTriageCat+1);
		}
	}
}

std::vector<IntVector> 
EvacMDP::getAllLegalPayloads(const Vehicle& vehicle) const
{
    std::vector<IntVector> legalPayloads;
    
    const auto capacityOfVehicle = vehicle.GetTotalCapacity();
    const auto popAtSite = CurrentState()->GetSite(vehicle.GetCurrentSiteLabel()).GetPopulation();
    const auto populationVector = popAtSite.GetPopulationVector();
    const auto capacityVector = popAtSite.GetCapacityVector();
    const auto numberOfCategories = popAtSite.GetPopulationVector().size();

    IntVector payload(numberOfCategories, 0); 

    recurvSolve(legalPayloads,
	    		payload,
                populationVector,
                capacityVector,
                capacityOfVehicle,
                0);

    return legalPayloads;
}

SiteVector 
EvacMDP::getAllSitesInRange(const Vehicle& vehicle) const
{
    SiteVector vectorOfSitesInRange;

    for(const auto& site : CurrentState()->GetSites())
    {
        // Skip the site where the vehicle currently is located
        if(site.GetLabel() == vehicle.GetCurrentSiteLabel()) continue;

        // Skip sites where the vehicle is unable to go
        if(vehicle.GetType() == VehicleType::FW && site.GetType() == SiteType::REMOTE) continue;

        const auto range = vehicle.GetRange();
        const auto dist2Site = calculateDist(vehicle.GetPosition(), site.GetPosition());

        if(range >= dist2Site) vectorOfSitesInRange.emplace_back(site);
    }

    return vectorOfSitesInRange;
}
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib> 

#include "EvacPolicy.h"
#include "EvacState.h"
#include "Task.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Conversion functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TaskSharedPtr
action2Task(const ActionSharedPtr& action)
{
    return std::dynamic_pointer_cast<Task>(action);
};

TaskSharedPtrVector
actionVector2TaskVector(const ActionSharedPtrVector& actionVector)
{
    TaskSharedPtrVector taskVector;

    for(const auto& action : actionVector)
    {
        taskVector.emplace_back(action2Task(action));
    }

    return taskVector;
};

EvacStateSharedPtr
state2EvacState(const StateSharedPtr& state)
{
    return std::dynamic_pointer_cast<EvacState>(state);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

EvacPolicy::EvacPolicy(const EvacPolicyType type)
{
    type_ = type;
}

EvacPolicy::~EvacPolicy()
{
    
}

TaskSharedPtrVector
filterLegalTasks(const Vehicle& v, const TaskSharedPtrVector& legalTasks)
{
    TaskSharedPtrVector filteredTasks;

    for(const auto& task : legalTasks)
        if(task->GetVehicleLabel() == v.GetLabel()) filteredTasks.emplace_back(task);

    return filteredTasks;
}

ActionSharedPtrVector
EvacPolicy::GetDecision(const ActionSharedPtrVector legalActions,
                        const StateSharedPtr state) const
{
    ActionSharedPtrVector tasks;

    auto legalTasks = actionVector2TaskVector(legalActions);
    
    for(const auto& v : state2EvacState(state)->GetVehicles())
    {
        if(v.GetStatus() != VehicleStatus::IDLE) continue;
           
        // Filter out legal tasks that arent assigned to the current vehicle
        const auto filteredTasks = filterLegalTasks(v, legalTasks);

        // Select one of the legal tasks that can be assigned to the curent vehicle
        auto newTask = selectTask(filteredTasks, state2EvacState(state));

        // Add the new task to the decision 
        if(newTask != nullptr) tasks.emplace_back(newTask);
    }

    return tasks;
}

ActionSharedPtrVector 
EvacPolicy::GetDecision(const TaskSharedPtrVector legalTasks,
                        const StateSharedPtr state) const
{
    ActionSharedPtrVector legalActions;
    for(const auto& task : legalTasks)
    {
        legalActions.emplace_back(task);
    }

    return GetDecision(legalActions, state);
}

void 
EvacPolicy::UpdateVFABackwardDP(const StateSharedPtrVector& trajectory,
                                const HistoryOfActions& actions)
{   
    HistoryOfTasks taskHistory;
    for(const auto& action : actions)
    {
        TaskSharedPtrVector tasks;

        for(const auto& a : action)
        {
            tasks.emplace_back(action2Task(a));
        }

        taskHistory.emplace_back(tasks);
    }

    EvacStateSharedPtrVector stateVector; 
    for(const auto& state : trajectory)
    {
        stateVector.emplace_back(state2EvacState(state));
    }
    
    updateVFABackwardDP(stateVector, taskHistory); 
}

void
EvacPolicy::UpdateVFAForwardDP( const StateSharedPtr& currentState,
                                const ActionSharedPtrVector& actions)
{
    const auto& tasks = actionVector2TaskVector(actions);
    updateVFAForwardDP(state2EvacState(currentState), tasks);
}            

EvacPolicyType 
EvacPolicy::GetType() const
{
    return type_;
}

void 
EvacPolicy::updateVFABackwardDP(const EvacStateSharedPtrVector& trajectory,
                                const HistoryOfTasks& tasks)
{

}

void 
EvacPolicy::updateVFAForwardDP(const EvacStateSharedPtr& currentState,
                                const TaskSharedPtrVector& task)
{

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

RandomPolicy::RandomPolicy()
: EvacPolicy(EvacPolicyType::RANDOM)
{
}

RandomPolicy::~RandomPolicy()
{
    
}


TaskSharedPtr
RandomPolicy::selectTask(   const TaskSharedPtrVector& legalTasks, 
                            const EvacStateSharedPtr state) const
{   
    TaskSharedPtrVector filteredTasks;

    for(uint i = 0; i < legalTasks.size(); i++)
    {
        const auto& t = legalTasks[i];

        if(t->GetType() == TaskType::RETURNTRANSPORT) filteredTasks.emplace_back(t);   
    }

    int randomIndex = rand() % static_cast<int>(filteredTasks.size());

    const auto selectedTask = filteredTasks[randomIndex];

    return selectedTask;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

PFAPolicy::PFAPolicy()
: EvacPolicy(EvacPolicyType::PFA)
{
}

PFAPolicy::~PFAPolicy()
{

}

TaskSharedPtr
PFAPolicy::selectTask(  const TaskSharedPtrVector& legalTasks, 
                        const EvacStateSharedPtr state) const
{
    TaskSharedPtr selectedTask;

    // IS population
    const auto popVec = state->GetSites()[0].GetPopulationVector();
    IntVector evac(4,0);
    int capRemaining = 10;

    // Rescue RED
    if(popVec[3] >= 3)
    {
        evac[3] = 3;
    }
    else
    {
        evac[3] = popVec[3];
    }
    capRemaining -= 3.*evac[3];
    
    
    // Rescue YELLOW
    if(capRemaining >= 3)
    {
        if(3.*popVec[2] <= capRemaining)
        {
            evac[2] = popVec[2]; 
        }
        else
        {
            evac[2] = std::floor(capRemaining / 3);
        }
        capRemaining -= 3.*evac[2];
    }

    // Rescue GREEN
    if(popVec[1] <= capRemaining)
    {
        evac[1] = popVec[1]; 
    }
    else
    {
        evac[1] = capRemaining;
    }
    capRemaining -= evac[1];
    
    
    // Rescue WHITE
    if(popVec[0] <= capRemaining)
    {
        evac[0] = popVec[0]; 
    }
    else
    {
        evac[0] = capRemaining;
    }
    
    for(const auto& task : legalTasks)
    {
        if(task->GetType() != TaskType::RETURNTRANSPORT) continue;

        const auto t = std::dynamic_pointer_cast<ReturnTransportTask>(task);

        const auto pv = t->GetPopulationVector();

        if( pv[0] == evac[0] &&
            pv[1] == evac[1] &&
            pv[2] == evac[2] &&
            pv[3] == evac[3] )
            {
                selectedTask = task;
                break;
            }

    }

    return selectedTask;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

UserPolicy::UserPolicy()
: EvacPolicy(EvacPolicyType::USER)
{
}

UserPolicy::~UserPolicy()
{
 
}

TaskSharedPtr 
UserPolicy::selectTask( const TaskSharedPtrVector& legalTasks, 
                        const EvacStateSharedPtr state) const
{
    state->Print();

    for(uint i = 0; i < legalTasks.size(); i++)
    {
        const auto& t = legalTasks[i];

        if(t->GetType() == TaskType::FERRY || t->GetType() == TaskType::TRANSPORT) continue;

        std::cout << i << " : ";
        t->Print();
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    }

    int c = 0;
    std::cin >> c;

    return legalTasks[c];
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

VFA4DPolicy::VFA4DPolicy(   const double learningRate, 
                            const double defaultVFA,
                            const double espilonGreedy)
: EvacPolicy(EvacPolicyType::VFA4D)
{
    defaultVFA_ = defaultVFA;
    alpha_ = learningRate;
    epsilon_ = espilonGreedy;
}

VFA4DPolicy::~VFA4DPolicy()
{
 
}

void 
VFA4DPolicy::Print() const
{
    for(const auto& p : valueFunctionApproximationLookupTable)
    {
        std::cout << p.first << "\t" << p.second << std::endl;
    }
}

bool 
VFA4DPolicy::IsLearning() const
{
    return learningMode_;
}

void 
VFA4DPolicy::SetLearningMode(const bool val)
{
    learningMode_ = val;
}

double 
getBellmanValueOfTasks(const TaskSharedPtrVector& decisions)
{
    double decisionValue = 0.;    
    
    for(const auto& decision : decisions)
    {
        if(decision == nullptr) continue;

        decisionValue  += (decision->GetReward() + decision->GetBellmanValue());
    }

    return decisionValue;
}

void 
VFA4DPolicy::updateVFABackwardDP(   const EvacStateSharedPtrVector& trajectory,
                                    const HistoryOfTasks& decisions)
{
    for(uint iState = trajectory.size()-1; iState > 0; iState--)
    {      
        const auto& state = trajectory[iState]; 
        const auto pdsv = getPostDecisionStateVariable(state);
        
        auto iter = valueFunctionApproximationLookupTable.find(pdsv);
        
        if(iter != valueFunctionApproximationLookupTable.end())
        {
            // new entry update
            const auto previousValue = iter->second;
            const auto decisionValue = getBellmanValueOfTasks(decisions[iState]);
            iter->second = (1. - alpha_) * previousValue + alpha_*decisionValue;
        }
        else
        {
            // do new entry
            valueFunctionApproximationLookupTable[pdsv] = defaultVFA_;
        }
    }
}

void 
VFA4DPolicy::updateVFAForwardDP(    const EvacStateSharedPtr& currentState,
                                    const TaskSharedPtrVector& decisions)
{
    const auto decisionValue = getBellmanValueOfTasks(decisions);
    
    if(decisions.size() == 0) return;
    if(decisions[0] == nullptr) return;

    PDSV pdsv =  getPostDecisionStateVariable(currentState, decisions[0]);
 
    auto iter = valueFunctionApproximationLookupTable.find(pdsv);
         
    if(iter != valueFunctionApproximationLookupTable.end())
    {
      
        const auto previousValue = iter->second;
        iter->second = (1. - alpha_) * previousValue + alpha_ * decisionValue;
    }
    else
    {
        const auto defaultEntry = std::pair<std::string, double>(pdsv,alpha_ * decisionValue);
        valueFunctionApproximationLookupTable.insert(defaultEntry);
    }
}

TaskSharedPtr 
VFA4DPolicy::selectTask(    const TaskSharedPtrVector& legalTasks, 
                            const EvacStateSharedPtr state) const
{
    TaskSharedPtr selectedTask;
    double maxValue = 0.;

    TaskSharedPtrVector filteredTasks;
    for(uint i = 0; i < legalTasks.size(); i++)
    {
        const auto& t = legalTasks[i];

        if(t->GetType() == TaskType::RETURNTRANSPORT) filteredTasks.emplace_back(t);   
    }

    double choice = 1.;
    if(learningMode_) choice  = rand();

    if(choice > epsilon_)
    {
        for(const auto task : filteredTasks)
        {
            const auto reward = task->GetReward();
            const auto pdsv = getPostDecisionStateVariable(state, task);
            const auto valueFunction = getValue(pdsv);

            auto result = reward + valueFunction;

            if(result > maxValue)
            {
                selectedTask = task;
                maxValue = result;
            }
        }
        selectedTask ->SetBellmanValue(maxValue);

    }
    else
    {
        int randomIndex = rand() % static_cast<int>(filteredTasks.size());
        selectedTask = filteredTasks[randomIndex];
    }
  
    return selectedTask;
}

double 
VFA4DPolicy::getValue(const PDSV& pdsv) const
{
     const auto iter = valueFunctionApproximationLookupTable.find(pdsv);

     if(iter != valueFunctionApproximationLookupTable.end())
     {
        return iter->second;
     } 
     else
     {
         return defaultVFA_;
     }
}

PDSV 
VFA4DPolicy::getPostDecisionStateVariable(  const EvacStateSharedPtr state, 
                                            TaskSharedPtr task) const
{
    PDSV pdsv;

    for(const auto& site : state2EvacState(state)->GetSites())
    {    
        if(site.IsGoal()) continue;
        
        const auto popAtSite  = site.GetPopulationVector();
        
        IntVector popEvac(popAtSite.size(), 0);

        
        if(task != nullptr)
        {
            if(task->GetType() == TaskType::RETURNTRANSPORT)
            {
                const auto t = std::dynamic_pointer_cast<ReturnTransportTask>(task);
                popEvac = t->GetPopulationVector();
            }
            if(task->GetType() == TaskType::TRANSPORT)
            {
                const auto t = std::dynamic_pointer_cast<TransportTask>(task);
                popEvac = t->GetPopulationVector();
            }
        }
    
        for(uint iCat = 0; iCat < popAtSite.size(); iCat++)
        {
            int c = popAtSite[iCat] - popEvac[iCat];
            pdsv.append(std::to_string(c)+",");
        }
    }

    return pdsv;
}
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <memory>
#include <chrono>
#include <ctime>        

#include "Simulation.h"

#include "EvacUtil.h"
#include "Site.h"
#include "Vehicle.h"
#include "Population.h"
#include "EvacPolicy.h"
#include "EvacMDP.h"
#include "EvacState.h"

// g++ --std=c++11 main.cpp src/* -Iinclude/ -I/home/nshiell/Documents/MyCode/MDPLibrary/include -o mdp

using namespace Util;

EvacMDPSharedPtr CreateSimpleMDP();

int main()
{
    std::srand( unsigned ( std::time(NULL) ) );

    VFA4DPolicySharedPtr agentVFA = std::make_shared<VFA4DPolicy>(0.05, 0., 0.5);
    EvacMDPSharedPtr mdp = CreateSimpleMDP();

    // Train the policy using forward dynamic programming
    Trainer train;
    train.AddAgent(agentVFA);
    train.AddMarkovDecisionProcess(mdp);

    agentVFA->SetLearningMode(true);

    train.TrainForwardDP(10000);
    //train.TrainBackwardDP(10000);

    // Evaluate the trained policy
    Evaluator eval;
    eval.AddMarkovDecisionProcess(mdp);
    eval.AddAgent(agentVFA);

    agentVFA->SetLearningMode(false);
    eval.Evaluate(1);

    for(const auto& action : eval.GetHistoryOfActions())
    {
        for(const auto& t : action)
        {
            std::dynamic_pointer_cast<Task>(t)->Print();
        }
    }

    for(const auto& r : eval.GetRewards()) std::cout << r << std::endl;
    
    //agentVFA->Print();

    return 0;
}

EvacMDPSharedPtr 
CreateSimpleMDP()
{
    // Define Population parameters
    DoubleVector meanLifeTimesExposed = {120., 48., 8., 1.5};  
    DoubleVector meanLifeTimesSheltered = {LARGE_NUMBER, LARGE_NUMBER, LARGE_NUMBER, LARGE_NUMBER};  
    IntVector capacityRequired = {1,1,3,3};

    // Create the incident site
    //IntVector incidentSite_ITD = {1900, 40, 30, 30};
    IntVector incidentSite_ITD = {100, 0, 100, 0};
    Population incidentSite_IP(incidentSite_ITD, meanLifeTimesExposed,capacityRequired);
    Position incidentSite_Pos = {225.,0.};
    Site incidentSite(  incidentSite_Pos,   // position
                        2000,               // capacity
                        "IS",               // label
                        incidentSite_IP,    // Initial Population
                        SiteType::REMOTE);  // Type

    // Create the forward operating base
    IntVector forwardOperatingBase_ITD = {0,0,0,0};
    Population forwardOperatingBase_IP(forwardOperatingBase_ITD, meanLifeTimesSheltered, capacityRequired);
    Position forwardOperatingBase_Pos = {0.,0.};
    Site forwardOperatingBase(  forwardOperatingBase_Pos,   // position
                                10000,                      // capacity
                                "FOB",                      // Label
                                forwardOperatingBase_IP,    // Initial Pop
                                SiteType::GOAL);            // Type
   
    // Create helicopter
    IntVector helo1_ITD = {0,0,0,0};
    Population helo1_IP(helo1_ITD, meanLifeTimesSheltered,capacityRequired);
    Position helo1_Pos = {0.,0.};

    Vehicle heloFast(   incidentSite,           // initial site
                        10,                     // capacity
                        "HeloFast",             // label
                        helo1_IP,               // initial pop
                        VehicleType::RW,        // type
                        150.,                   // speed [km/h]
                        600.);                  // range [km]

    EvacStateSharedPtr state = std::make_shared<EvacState>();
    state->AddSite(incidentSite);
    state->AddSite(forwardOperatingBase);
    state->AddVehicle(heloFast);

    // Create the actual simulation
    EvacMDPSharedPtr mdp = std::make_shared<EvacMDP>();
    mdp->SetInitialState(state);

    return mdp;
}

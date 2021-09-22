#ifndef EVAC_MDP_H
#define EVAC_MDP_H

#include "MarkovDecisionProcess.h"
#include "Util.h"
#include "EvacUtil.h"

class EvacMDP : public MarkovDecisionProcess
{
public:

    EvacMDP();
    ~EvacMDP();

    void Return2InitialState();

    void DecisionUpdate(const ActionSharedPtrVector& actions);
    void ExogenousUpdate();

    void UpdateTotalReward();

    bool IsEndState() const;

    ActionSharedPtrVector GetLegalActions() const;

    void Print() const;

    EvacStateSharedPtr CurrentState() const;
    EvacStateSharedPtr InitialState() const;

private:

    double calculateExpectedReward( const StateSharedPtr state, 
                                    const ActionSharedPtr decision);

    TaskSharedPtrVector getLegalTasks(const Vehicle& vehicle) const;
    void handleLegalTask_Idle(const Vehicle& vehicle, TaskSharedPtrVector& legalTasks) const;
    void handleLegalTask_Transport(const Vehicle& vehicle, TaskSharedPtrVector& legalTasks) const;
    void handleLegalTask_Ferry(const Vehicle& vehicle, TaskSharedPtrVector& legalTasks) const;
    void handleLegalTask_Refit(const Vehicle& vehicle, TaskSharedPtrVector& legalTasks) const;

    SiteVector getAllSitesInRange(const Vehicle& vehicle) const;
    std::vector<IntVector> getAllLegalPayloads(const Vehicle& vehicle) const;
};

#endif
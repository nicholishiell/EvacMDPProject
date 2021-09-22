#ifndef EVAC_POLICY_H
#define EVAC_POLICY_H

#include "Agent.h"
#include "Util.h"
#include "EvacUtil.h"

class EvacPolicy : public Agent
{
public : 
     
    EvacPolicy(const EvacPolicyType type);
    ~EvacPolicy();

    ActionSharedPtrVector GetDecision(  const ActionSharedPtrVector legalActions,
                                        const StateSharedPtr state) const;

    ActionSharedPtrVector GetDecision(  const TaskSharedPtrVector legalTasks,
                                        const StateSharedPtr state) const;

    virtual void UpdateVFABackwardDP(   const StateSharedPtrVector& trajectory,
                                        const HistoryOfActions& actions);
    
    virtual void UpdateVFAForwardDP(    const StateSharedPtr& currentState,
                                        const ActionSharedPtrVector& actions);

    EvacPolicyType GetType() const;

private:

    virtual TaskSharedPtr selectTask(   const TaskSharedPtrVector& legalTasks,
                                        const EvacStateSharedPtr state) const = 0;

    virtual void updateVFABackwardDP(   const EvacStateSharedPtrVector& trajectory,
                                        const HistoryOfTasks& tasks);
    
    virtual void updateVFAForwardDP(   const EvacStateSharedPtr& currentState,
                                        const TaskSharedPtrVector& task);

    EvacPolicyType type_;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class RandomPolicy : public EvacPolicy
{
public:

    RandomPolicy();
    ~RandomPolicy();

private:

    TaskSharedPtr selectTask(   const TaskSharedPtrVector& legalTasks,
                                const EvacStateSharedPtr state) const;

};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class PFAPolicy : public EvacPolicy
{
public:

    PFAPolicy();
    ~PFAPolicy();

private:

    TaskSharedPtr selectTask(   const TaskSharedPtrVector& legalTasks,
                                const EvacStateSharedPtr state) const;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class UserPolicy : public EvacPolicy
{
public:

    UserPolicy();
    ~UserPolicy();

private:
   
    TaskSharedPtr selectTask(   const TaskSharedPtrVector& legalTasks,
                                const EvacStateSharedPtr state) const;

};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class VFA4DPolicy : public EvacPolicy
{

public:

    VFA4DPolicy(const double learningRate, 
                const double defaultVFA, 
                const double espilonGreedy = -1.);
    ~VFA4DPolicy();

    void Print() const;

    bool IsLearning() const;
    void SetLearningMode(const bool val);

private:
   
    TaskSharedPtr selectTask(   const TaskSharedPtrVector& legalTasks,
                                const EvacStateSharedPtr state) const;

    void updateVFABackwardDP(   const EvacStateSharedPtrVector& trajectory,
                                const HistoryOfTasks& decisions);

    void updateVFAForwardDP(   const EvacStateSharedPtr& currentState,
                                const TaskSharedPtrVector& decisions);


    PDSV getPostDecisionStateVariable(  const EvacStateSharedPtr state, 
                                        TaskSharedPtr task = nullptr) const; 

    double getValue(const PDSV& pdsv) const;

    std::unordered_map<std::string, double> valueFunctionApproximationLookupTable;

    double defaultVFA_;   
    double alpha_;
    double epsilon_;

    bool learningMode_;
};

#endif
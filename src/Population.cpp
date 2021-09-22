#include <iostream>

#include "Population.h"

Population::Population()
{
    //std::srand( unsigned ( std::time(NULL) ) );
}

Population::Population( const IntVector& initialPopulation, 
                        const DoubleVector& meanLifeTimes,
                        const IntVector& capacityRequired)
{   
    population_ = initialPopulation;
    meanLifeTimes_ = meanLifeTimes;
    capcityRequired_ = capacityRequired;

    numberOfTriageCats_ = static_cast<int>(population_.size()) - 1;

    numberOfDeaths_ = 0;
}

Population::~Population()
{
    
}

void
Population::Print() const
{
    for(const auto v : population_)
        std::cout <<  v <<" ";
    std::cout << std::endl;
}

bool 
Population::AddPopulation(const IntVector& pop)
{
    if(pop.size() != population_.size()) return false;

    for(uint iTriage = 0; iTriage < pop.size(); iTriage++)
    {
        population_[iTriage] += pop[iTriage];
    }

    return true;
}

bool 
Population::RemovePopulation(const IntVector& pop)
{
    if(pop.size() != population_.size()) return false;

    for(uint iTriage = 0; iTriage < pop.size(); iTriage++)
    {
        if(pop[iTriage] > population_[iTriage]) return false;
        
        population_[iTriage] -= pop[iTriage];
    }

    return true;
}

IntVector 
Population::GetPopulationVector() const
{
    return population_;
}

IntVector 
Population::GetCapacityVector() const
{
    return capcityRequired_;
}

void
Population::Update(const double deltaTime)
{  
    const auto transitionProbability = caclulateTransitionProbabilities(deltaTime);

    for(int iTriage = numberOfTriageCats_; iTriage >= 0; iTriage--)
    {
        int numberOfTransitions = 0;
        for(int iPerson = 0; iPerson < population_[iTriage]; iPerson++)
        {
            const auto rn = ((double) rand() / (RAND_MAX));
            if(rn < transitionProbability[iTriage]) numberOfTransitions++; 
        }

        if(iTriage == numberOfTriageCats_)
        {
            population_[iTriage] -= numberOfTransitions;
            numberOfDeaths_ += numberOfTransitions;
        }
        else
        {
            population_[iTriage+1] += numberOfTransitions;
            population_[iTriage] -= numberOfTransitions;
        }
    }
}

DoubleVector 
Population::caclulateTransitionProbabilities(const double& dt) const
{
    DoubleVector transitionProbability;

    for(const auto mu : meanLifeTimes_)
    {
        transitionProbability.emplace_back( 1. - std::exp(-1.*dt/mu) );
    }

    return transitionProbability;
}
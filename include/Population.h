#ifndef POPULATION_H
#define  POPULATION_H

#include <math.h>
#include <random>
#include <vector>

#include "Util.h"
#include "EvacUtil.h"

using namespace Util;

class Population
{
public:
    Population();

    Population( const IntVector& initialPopulation, 
                const DoubleVector& meanLifeTimes,
                const IntVector& capacityRequired);

    ~Population();

    void Update(const double deltaTime);

    bool AddPopulation(const IntVector& pop);
    bool RemovePopulation(const IntVector& popIn);

    IntVector GetPopulationVector() const;
    IntVector GetCapacityVector() const;

    void Print() const;

private:

    DoubleVector caclulateTransitionProbabilities(const double& dt) const;

    IntVector population_;
    IntVector capcityRequired_;
    DoubleVector meanLifeTimes_;

    int numberOfTriageCats_;

    int numberOfDeaths_;
};

#endif
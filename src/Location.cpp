#include <iostream>

#include "Location.h"

Location::Location()
{
    
}

Location::Location( const Util::Position& pos, 
                    const int cap, 
                    const std::string& label, 
                    const Population& initialPopulation)
{
    totalCapacity_= cap;
    swingCapacity_ = totalCapacity_;

    position_ = pos;

    population_ = initialPopulation;

    label_ = label;
}

Location::~Location()
{
    
}

void
Location::Update(const double deltaTime)
{   
    population_.Update(deltaTime);   
    typeSpecificUpdate(deltaTime);
}

void 
Location::UpdateCapacity(const int newSwingCap, const int newNonCriticalCap, const int newCriticalCap)
{
    if(newSwingCap + newNonCriticalCap + newCriticalCap == totalCapacity_)
    {
        swingCapacity_ = newSwingCap;
        noncriticalCapacity_ = newNonCriticalCap;
        criticalCapacity_ = newCriticalCap;
    }
    else if(newSwingCap + newNonCriticalCap + newCriticalCap < totalCapacity_)
    {
        noncriticalCapacity_ = newNonCriticalCap;
        criticalCapacity_ = newCriticalCap;
                                                               
        swingCapacity_ = totalCapacity_ - newNonCriticalCap -  criticalCapacity_;
    }
    else
    {
        std::cout << "WARNING: Capacity could not be updated." << std::endl;
    }
}

std::string 
Location::GetLabel() const
{
    return label_;
}

Position 
Location::GetPosition() const
{
    return position_;
}

IntVector
Location::GetPopulationVector() const
{
    return population_.GetPopulationVector();
}

Population 
Location::GetPopulation() const
{
    return population_;
}

void 
Location::UpdatePosition(const Position& pos)
{
    position_ = pos;
}

void 
Location::RemovePopulation(const IntVector& pop)
{
    population_.RemovePopulation(pop);
}

void 
Location::AddPopulation(const IntVector& pop)
{
    population_.AddPopulation(pop);
}

int 
Location::GetTotalCapacity() const
{
    return totalCapacity_;
}

#include <iostream>

#include "Site.h"

Site::Site()
{
    
}

Site::Site( const Util::Position& pos, 
            const int cap, 
            const std::string& label, 
            const Population& initialPopulation,
            const SiteType type)
: Location(pos, cap, label, initialPopulation)
{
    type_ = type;
}

Site::~Site()
{

    
}

void 
Site::Print() const
{
    std::cout << "~~~~~~~~~~~~ SITE ~~~~~~~~~~~~~~" << std::endl;
    std::cout << "Name: " << GetLabel() << std::endl;
    std::cout << "Position: " << GetPosition()[0] << "," << GetPosition()[1] << std::endl;
    std::cout << "Capacity: " << GetTotalCapacity() << std::endl;
    std::cout << "Population: " << IntVector2String(GetPopulationVector())  << std::endl;
}

SiteType
Site::GetType() const
{
    return type_;
}

bool 
Site::IsGoal() const
{
    return type_ == SiteType::GOAL;
}

void
Site::typeSpecificUpdate(const double deltaTime)
{

}


#ifndef SITE_H
#define  SITE_H

#include "Util.h"
#include "EvacUtil.h"
#include "Location.h"

class Site : public Location
{
public:
    Site();

    Site(   const Util::Position& pos, 
            const int cap, 
            const std::string& label,
            const Population& initialPopulation,
            const SiteType type);
    ~Site();

    SiteType GetType() const;
    bool IsGoal() const;  

    void Print() const;

private:

    void typeSpecificUpdate(const double deltaTime);

    SiteType type_;
};

#endif
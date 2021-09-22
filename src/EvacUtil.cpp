#include <iostream>

#include "EvacUtil.h"

namespace Util
{

std::string
IntVector2String(const IntVector vec)
{
    std::string str;
    for(const auto v : vec)
    {
        str.append(std::to_string(v));
        str.append(" ");
    }
    return str;
}

double 
calculateDist(const Position& p1, const Position& p2)
{
    const auto result = pow(p1[0] - p2[0],2) + pow(p1[1] - p2[1],2);
    return sqrt(result);
}

Heading 
calculateHeading(const Position& currentPos, const Position& destinationPos)
{
    Heading h;
    auto dist = calculateDist(currentPos,destinationPos);

    h[0] = (destinationPos[0] - currentPos[0]) / dist;
    h[1] = (destinationPos[1] - currentPos[1]) / dist;

    return h;
}

int 
sumVector(const IntVector& array) 
{
    int sum = 0;
    for(const auto& v : array) sum += v;
    return sum;
}

std::string 
TaskType2String(const TaskType tt)
{
    if(tt == TaskType::IDLE) return "IDLE";
    else if(tt == TaskType::TRANSPORT) return "TRANSPORT";
    else if(tt == TaskType::FERRY) return "FERRY";
    else if(tt == TaskType::REFIT) return "REFIT";
    else return "!!! ERROR !!!";
}

}
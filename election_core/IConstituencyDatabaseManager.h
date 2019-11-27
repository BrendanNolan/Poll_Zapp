#ifndef ICONSTITUENCYDATABASEMANAGER_H
#define ICONSTITUENCYDATABASEMANAGER_H

#include <memory>
#include <vector>

class Constituency;

class IConstituencyDatabaseManager
{
public:
    virtual ~IConstituencyDatabaseManager() = default;

    virtual void init() const = 0;
    virtual void addConstituency(Constituency& constituency) const = 0;
    virtual void updateConstituency(const Constituency& constituency) const = 0;
    virtual void removeConstituency(int id) const = 0;
    virtual std::vector<std::unique_ptr<Constituency>> 
        constituencies() const = 0;
};

#endif // ICONSTITUENCYDATABASEMANAGER_H

export module Engine.Entity;


import std;

export using Entity = std::uint32_t;

export class EntityManager
{
    //std::vector<Entity> entities;
    Entity nextEntity = 0;

public:

    Entity CreateEntity()
    {
        return nextEntity++; // return and then increment the value
    }

    std::uint32_t GetEntityCount()
    {
        return nextEntity;
    }
};

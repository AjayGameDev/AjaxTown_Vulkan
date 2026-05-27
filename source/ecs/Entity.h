#pragma once


using Entity = uint32_t;

class EntityManager
{
    //std::vector<Entity> entities;
    Entity nextEntity = 0;

public:

    Entity CreateEntity()
    {
        return nextEntity++; // return and then increment the value
    }

    uint32_t GetEntityCount()
    {
        return nextEntity;
    }
};

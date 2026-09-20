#pragma once
#include "Entity.h"


template <typename T>
class ComponentManager
{
    std::unordered_map<Entity,T> data;

public:

    void AddComponent(Entity e,T& t)
    {
        data.emplace(e,std::move(t));
    }

    T& GetComponent(Entity e)
    {
        if (data.contains(e))
            return data.at(e);

        spdlog::error("Component doesn't exist for entity " + std::to_string(e));
        throw std::runtime_error("Component doesn't exist for entity " + std::to_string(e));
    }
    void RemoveComponent(Entity e)
    {
        if (data.contains(e))
            data.erase(e);
        else
            spdlog::error("Component doesn't exist for this entity " + std::to_string(e));
    }

    void DebugInfo(std::ostream& out)
    {
        std::string componentName = typeid(T).name();
        if (componentName.starts_with("struct "))
            componentName.erase(0,7);
        else if (componentName.starts_with("class "))
            componentName.erase(0,6);

        for (auto& [key,value] : data)
        {
            std::cout << "\n" << "Entity " << key << " has " << componentName << " component " ;
            value.Serialize(out);
        }
    }
};
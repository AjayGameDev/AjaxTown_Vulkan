export module Engine.ComponentManager;


import Engine.Entity;
import Engine.Component.Transform;
import std;


export template <typename T>
class ComponentManager
{
    std::unordered_map<Entity,T> data;

public:

    void AddComponent(Entity e,T t)
    {
        data[e] = t;
    }

    T& GetComponent(Entity e)
    {
        return data[e];
    }
    void RemoveComponent(Entity e)
    {
        data.erase(e);
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
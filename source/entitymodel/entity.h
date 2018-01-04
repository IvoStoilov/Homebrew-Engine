#pragma once
#include "system/math/mat4x4.h"

#include <vector>
class Component;
class Entity
{
public:
    Entity();
    ~Entity();

    inline Transform& GetGlobalMatrix() { return m_GlobalMatrix; }
    inline void SetGlobalMatrix(const Transform& globalMatrix) { m_GlobalMatrix = globalMatrix; }
    inline void SetGlobalPosition(const vec4& pos) { m_GlobalMatrix.SetTranslate(pos); }

    void Update(float dt);
    void OnDestroy();

    void Rotate(float degree);

    void AddComponent(Component* component);
    template<typename T>
    T* GetComponentByType()
    {
        for (const auto& component : m_Components)
        {
            T* result = dynamic_cast<T*>(component);
            if (result)
                return result;
        }
        return nullptr;

    }
private:
    std::vector<Component*> m_Components;

    Transform m_GlobalMatrix;

    int32_t m_Angle;
};
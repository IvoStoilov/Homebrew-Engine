#pragma once

class Component;
class Entity
{
public:
    Entity();
    ~Entity();

    inline mat4x4& GetGlobalMatrix() { return m_GlobalMatrix; }
    inline const mat4x4& GetGlobalMatrix() const { return m_GlobalMatrix; }
    inline void SetGlobalMatrix(const mat4x4& globalMatrix) { m_GlobalMatrix = globalMatrix; }

    inline void SetGlobalPosition(const vec4& pos) { m_GlobalMatrix.SetTranslation(pos); }

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
    mat4x4 m_GlobalMatrix;
    float m_Angle;
};
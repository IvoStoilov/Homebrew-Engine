#pragma once
class Entity;
class Component
{
public:
    Component();
    virtual ~Component();

    virtual void Initialize();
    virtual void Update(float dt);

    virtual void OnAddToWorld();
    virtual void OnRemoveFromWorld();

    inline void SetOwner(Entity* owner) { m_Owner = owner; }
    inline Entity* GetOwner() const { return m_Owner; }

private:
    Entity* m_Owner;
};
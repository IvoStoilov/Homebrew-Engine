#include <engine/precompile.h>
#include <engine/entitymodel/entity.h>
#include <engine/entitymodel/components/component.h>

Entity::Entity() :
    m_Angle(0.f)
{}

Entity::~Entity()
{}

void Entity::Update(float dt)
{
    Rotate(m_Angle);
    m_Angle += 6.f * dt;
    if (m_Angle > 360.f)
        m_Angle = 0.f;
}

void Entity::Rotate(float deg)
{
    f32 rad = MathUtil::ToRads(deg);
    vec4 globalPos = m_GlobalMatrix.GetTranslation();
    m_GlobalMatrix = mat4x4::CreateRotationY(rad);
    m_GlobalMatrix.SetTranslation(globalPos);
}

void Entity::AddComponent(Component* component)
{
    if (component)
    {
        component->SetOwner(this);
        m_Components.push_back(component);
    }
}

void Entity::OnDestroy()
{
    for (Component* component : m_Components)
    {
        component->OnRemoveFromWorld();
        delete component;
    }
    m_Components.clear();
}
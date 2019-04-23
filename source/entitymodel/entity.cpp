#include "entitymodel/entity.h"
#include "entitymodel/components/component.h"

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
    float rad = MathUtil::ToRads(deg);
    vec4 globalPos = m_GlobalMatrix.GetTranslate();
    m_GlobalMatrix = mat4x4::GetRotateYAxisMatrix(rad);
    m_GlobalMatrix.SetTranslate(globalPos);
    //m_GlobalMatrix.NormalizeCollums();
    
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
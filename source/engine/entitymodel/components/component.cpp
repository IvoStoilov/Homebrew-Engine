#include <engine/precompile.h>
#include <engine/entitymodel/components/component.h>
#include <engine/entitymodel/entity.h>

Component::Component() :
    m_Owner(nullptr)
{}

Component::~Component()
{}

void Component::Initialize()
{}

void Component::Update(float dt)
{}

void Component::OnAddToWorld()
{}

void Component::OnRemoveFromWorld()
{}

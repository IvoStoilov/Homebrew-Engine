#include "entitymodel/entity.h"

Entity::Entity()
{}

Entity::~Entity()
{}

void Entity::Rotate(float deg)
{
    float rad = MathUtil::ToRads(deg);
    m_GlobalMatrix = mat4x4::GetRotateYAxisMatrix(rad);
    //m_GlobalMatrix.NormalizeCollums();
    
}
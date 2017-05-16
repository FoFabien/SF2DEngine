#include "../engine/game/entity.hpp"

#ifdef _USE_ENTITY_
void Entity::jump(int32_t force)
{
    if(!physic || physic->getType() != COMP_JUMP) return;

    ((JumpPhysicComponent*)physic)->grounded = false;
    ((JumpPhysicComponent*)physic)->time_air = -force;
}

bool Entity::isOnGround() const
{
    if(!physic || physic->getType() != COMP_JUMP) return false;
    return ((JumpPhysicComponent*)physic)->grounded;
}
#endif // _USE_ENTITY_

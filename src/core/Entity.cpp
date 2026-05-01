#include "Entity.h"
#include <algorithm>

// Hàm xóa vật thể
void removeEntity(std::vector<Entity*>& entities, Entity* entity) {
    entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
}

#include "TerrainMap.h"
#include <cstdlib>
#include <stdexcept>
#include <utility>
#include "TerrainProxy.h"
#include "Terrains.h"

TerrainMap::TerrainMap(int width, int height) : width_(width), height_(height) {
    if (width <= 0 || height <= 0)
        throw std::invalid_argument("TerrainMap: размеры карты должны быть > 0");

    cells_.resize(static_cast<std::size_t>(height));
    for (auto& row : cells_) {
        row.resize(static_cast<std::size_t>(width));
        for (auto& cell : row)
            cell = std::make_unique<TerrainProxy>(std::make_unique<Plains>());
    }
    objects_.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
}

bool TerrainMap::inBounds(int x, int y) const {
    return x >= 0 && y >= 0 && x < width_ && y < height_;
}

std::size_t TerrainMap::index(int x, int y) const {
    return static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) + static_cast<std::size_t>(x);
}

const Terrain& TerrainMap::at(int x, int y) const {
    if (!inBounds(x, y)) throw std::out_of_range("TerrainMap::at: клетка вне карты");
    return *cells_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
}

void TerrainMap::setTerrain(int x, int y, std::unique_ptr<Terrain> real) {
    if (!inBounds(x, y)) return;
    cells_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] =
        std::make_unique<TerrainProxy>(std::move(real));
}

void TerrainMap::generate() {
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            int r = std::rand() % 100;
            if (r < 72) setTerrain(x, y, std::make_unique<Plains>());
            else if (r < 80) setTerrain(x, y, std::make_unique<Forest>());
            else if (r < 92) setTerrain(x, y, std::make_unique<Swamp>());
            else setTerrain(x, y, std::make_unique<Mountains>());
        }
    }
}

bool TerrainMap::placeObject(std::unique_ptr<NeutralObject> object) {
    if (!object || !inBounds(object->x(), object->y())) return false;
    std::size_t i = index(object->x(), object->y());
    if (objects_[i]) return false;
    objects_[i] = std::move(object);
    return true;
}

NeutralObject* TerrainMap::getNeutralObjectAt(int x, int y) const {
    if (!inBounds(x, y)) return nullptr;
    return objects_[index(x, y)].get();
}

void TerrainMap::removeObjectAt(int x, int y) {
    if (inBounds(x, y)) objects_[index(x, y)].reset();
}

void TerrainMap::clearObjects() {
    for (auto& object : objects_) object.reset();
}

ObjectIterator TerrainMap::begin() const {
    return ObjectIterator(objects_.begin(), objects_.end());
}

ObjectIterator TerrainMap::end() const {
    return ObjectIterator(objects_.end(), objects_.end());
}

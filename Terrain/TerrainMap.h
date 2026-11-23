#pragma once
#include <memory>
#include <vector>
#include "NeutralObject.h"
#include "ObjectIterator.h"
#include "Terrain.h"

class TerrainMap {
public:
    TerrainMap(int width, int height);

    int width() const { return width_; }
    int height() const { return height_; }
    bool inBounds(int x, int y) const;

    const Terrain& at(int x, int y) const;
    void setTerrain(int x, int y, std::unique_ptr<Terrain> real);
    void generate();

    bool placeObject(std::unique_ptr<NeutralObject> object);
    NeutralObject* getNeutralObjectAt(int x, int y) const;
    void removeObjectAt(int x, int y);
    void clearObjects();

    ObjectIterator begin() const;
    ObjectIterator end() const;

private:
    std::size_t index(int x, int y) const;

    int width_;
    int height_;
    std::vector<std::vector<std::unique_ptr<Terrain>>> cells_;
    std::vector<std::unique_ptr<NeutralObject>> objects_;
};

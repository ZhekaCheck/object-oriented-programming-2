#include "GameField.h"
#include <stdexcept>
#include <utility>

GameField::GameField(int width, int height, std::size_t maxUnits)
    : width_(width), height_(height), maxUnits_(maxUnits), count_(0) {
    if (width <= 0 || height <= 0)
        throw std::invalid_argument("GameField: размеры поля должны быть > 0");
    cells_.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
}

GameField::~GameField() = default;

GameField::GameField(const GameField& other)
    : width_(other.width_), height_(other.height_),
      maxUnits_(other.maxUnits_), count_(other.count_) {
    cells_.reserve(other.cells_.size());
    for (const auto& cell : other.cells_) {
        if (cell) cells_.push_back(cell->clone()); // копируем самих юнитов, а не указатели
        else cells_.push_back(nullptr);
    }
}

GameField& GameField::operator=(const GameField& other) {
    if (this != &other) {
        GameField copy(other);
        *this = std::move(copy);
    }
    return *this;
}

GameField::GameField(GameField&& other) noexcept
    : width_(other.width_), height_(other.height_),
      maxUnits_(other.maxUnits_), count_(other.count_),
      cells_(std::move(other.cells_)) {
    other.width_ = 0;
    other.height_ = 0;
    other.maxUnits_ = 0;
    other.count_ = 0;
    other.cells_.clear();
}

GameField& GameField::operator=(GameField&& other) noexcept {
    if (this != &other) {
        width_ = other.width_;
        height_ = other.height_;
        maxUnits_ = other.maxUnits_;
        count_ = other.count_;
        cells_ = std::move(other.cells_);

        other.width_ = 0;
        other.height_ = 0;
        other.maxUnits_ = 0;
        other.count_ = 0;
        other.cells_.clear();
    }
    return *this;
}

bool GameField::inBounds(int x, int y) const {
    return x >= 0 && y >= 0 && x < width_ && y < height_;
}

// ячейки лежат в одном векторе построчно: индекс = y * ширина + x
std::size_t GameField::index(int x, int y) const {
    return static_cast<std::size_t>(y) * static_cast<std::size_t>(width_)
         + static_cast<std::size_t>(x);
}

bool GameField::addUnit(std::unique_ptr<Unit> unit) {
    if (!unit) return false;
    if (count_ >= maxUnits_) return false;
    if (!inBounds(unit->x(), unit->y())) return false;

    auto& cell = cells_[index(unit->x(), unit->y())];
    if (cell) return false;

    cell = std::move(unit);
    ++count_;
    return true;
}

bool GameField::addUnit(const UnitFactory& factory, UnitType type, int x, int y) {
    return addUnit(factory.createUnit(type, x, y));
}

bool GameField::removeUnit(int x, int y) {
    if (!inBounds(x, y)) return false;
    auto& cell = cells_[index(x, y)];
    if (!cell) return false;
    cell.reset();
    --count_;
    return true;
}

bool GameField::removeByIndex(std::size_t cellIndex) {
    if (cellIndex >= cells_.size() || !cells_[cellIndex]) return false;
    cells_[cellIndex].reset();
    --count_;
    return true;
}

Unit* GameField::getUnit(int x, int y) {
    return inBounds(x, y) ? cells_[index(x, y)].get() : nullptr;
}

const Unit* GameField::getUnit(int x, int y) const {
    return inBounds(x, y) ? cells_[index(x, y)].get() : nullptr;
}

bool GameField::moveUnit(int fromX, int fromY, int toX, int toY) {
    if (!inBounds(fromX, fromY) || !inBounds(toX, toY)) return false;
    auto& from = cells_[index(fromX, fromY)];
    auto& to = cells_[index(toX, toY)];
    if (!from || to) return false;
    from->moveTo(toX, toY, width_, height_);
    to = std::move(from);
    return true;
}

std::size_t GameField::size() const { return count_; }
bool GameField::empty() const { return count_ == 0; }
int GameField::width() const { return width_; }
int GameField::height() const { return height_; }
std::size_t GameField::maxUnits() const { return maxUnits_; }

void GameField::clear() {
    for (auto& cell : cells_) cell.reset();
    count_ = 0;
}

GameField::iterator GameField::begin() { return iterator(cells_.begin(), cells_.end()); }
GameField::iterator GameField::end() { return iterator(cells_.end(), cells_.end()); }

GameField::const_iterator GameField::begin() const { return const_iterator(cells_.begin(), cells_.end()); }
GameField::const_iterator GameField::end() const { return const_iterator(cells_.end(), cells_.end()); }

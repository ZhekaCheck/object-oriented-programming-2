#include <algorithm>
#include <cctype>
#include <chrono>
#include <clocale>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

#include "ConcreteFactories.h"
#include "GameController.h"
#include "Logger.h"
#include "TerrainProxy.h"

const int kMaxWidth = 35;
const int kMaxHeight = 40;
const int kFrameDelay = 400;

int menuPage = 0;

const UnitType kAllTypes[10] = {
    UnitType::Swordsman, UnitType::Spearman,
    UnitType::LightArcher, UnitType::HeavyArcher,
    UnitType::Cataphract, UnitType::HorseArcher,
    UnitType::FireMage, UnitType::IceMage,
    UnitType::Gryphon, UnitType::Wyvern
};

void enableColors() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(out, &mode))
        SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

void clearScreen() {
    std::cout << "\x1b[2J\x1b[3J\x1b[H" << std::flush;
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
}

UnitType randomType() { return kAllTypes[std::rand() % 10]; }

char symbolOf(UnitType type) {
    switch (type) {
    case UnitType::Swordsman: return 'M';
    case UnitType::Spearman: return 'S';
    case UnitType::LightArcher: return 'L';
    case UnitType::HeavyArcher: return 'H';
    case UnitType::Cataphract: return 'K';
    case UnitType::HorseArcher: return 'E';
    case UnitType::FireMage: return 'F';
    case UnitType::IceMage: return 'I';
    case UnitType::Gryphon: return 'G';
    case UnitType::Wyvern: return 'W';
    }
    return '?';
}

char unitSymbol(const Unit& u) {
    char c = symbolOf(u.getType());
    return u.isAlive() ? c : static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}

int colorOf(char c) {
    switch (c) {
    case '*': return 32;
    case '&': return 92;
    case '~': return 34;
    case '^': return 90;
    case 'B': return 97;
    case 'C': return 33;
    case 'T': return 36;
    case 'P': return 31;
    case 'R': return 94;
    case 'X': return 37;
    }
    return std::islower(static_cast<unsigned char>(c)) ? 90 : 97;
}

std::string paint(char c) {
    return "\x1b[" + std::to_string(colorOf(c)) + "m" + std::string(1, c) + "\x1b[0m";
}

std::vector<std::string> renderMap(const GameController& game) {
    const TerrainMap& map = game.map();
    std::vector<std::string> lines;

    std::string border;
    for (int i = 0; i < map.width() + 2; ++i) {
        border += paint('X');
        if (i < map.width() + 1) border += ' ';
    }

    lines.push_back(border);
    for (int y = 0; y < map.height(); ++y) {
        std::string row = paint('X') + " ";
        for (int x = 0; x < map.width(); ++x) {
            char c = map.at(x, y).symbol();
            if (const NeutralObject* object = map.getNeutralObjectAt(x, y)) c = object->symbol();
            if (x == game.base().x() && y == game.base().y()) c = 'B';
            if (const Unit* unit = game.field().getUnit(x, y)) c = unitSymbol(*unit);
            row += paint(c) + " ";
        }
        row += paint('X');
        lines.push_back(row);
    }
    lines.push_back(border);
    return lines;
}

std::vector<std::string> menuLines() {
    if (menuPage == 0) {
        return {
            "МЕНЮ 1/2 - ЛР 1: ОСНОВНОЕ",
            "1. Добавить юнита (фабрика)",
            "2. Удалить юнита (x, y)",
            "3. Удалить юнита (индекс ячейки)",
            "4. Переместить юнита",
            "5. Информация о юните",
            "6. Нанести урон юниту",
            "7. Атака: юнит бьёт юнита",
            "8. Массовый урон всем (итератор)",
            "9. Убрать погибших (итератор)",
            "10. Список юнитов (итератор)",
            "ЛР 1: ПРОЧЕЕ",
            "11. Случайная армия (фабрики)",
            "12. Очистить поле",
            "13. Сценарий 1: ядерный удар",
            "14. Сценарий 2: местная междоусобица",
            "15. Сценарий 3: типовая мировая война",
            "16. Создать новое поле",
            "",
            "99. Переключить на меню ЛР 2",
            "0. Выход"
        };
    }
    return {
        "МЕНЮ 2/2 - ЛР 2: БАЗА И КАРТА",
        "17. Создать юнита на базе",
        "18. Ход по ландшафту",
        "19. Закончить ход",
        "20. Проверка ландшафта (Proxy)",
        "21. Нанести урон базе",
        "22. Информация о базе",
        "23. Журнал событий",
        "ЛР 2: СЦЕНАРИИ",
        "24. Сценарий 4: база и юниты",
        "25. Сценарий 5: ландшафт и Proxy",
        "26. Сценарий 6: нейтр. объекты",
        "27. Сценарий 7: гибель юнита",
        "28. Автотесты",
        "",
        "99. Переключить на меню ЛР 1",
        "0. Выход"
    };
}

void drawScreen(const GameController& game, const std::string& message) {
    clearScreen();

    std::vector<std::string> left = renderMap(game);
    std::vector<std::string> right = menuLines();
    std::size_t leftWidth = static_cast<std::size_t>(2 * game.map().width() + 3);
    std::size_t rows = std::max(left.size(), right.size());

    for (std::size_t i = 0; i < rows; ++i) {
        std::string l = i < left.size() ? left[i] : std::string(leftWidth, ' ');
        std::cout << l << "    " << (i < right.size() ? right[i] : std::string()) << "\n";
    }

    const Base& base = game.base();
    std::cout << "* равнина  & лес  ~ болото  ^ горы | B база  C сундук  T ловушка  P алтарь  R источник\n"
        << "M S L H K E F I G W - юниты (строчная = погиб)\n"
        << "Поле " << game.map().width() << "x" << game.map().height() << ", юнитов: " << game.field().size()
        << "/" << game.field().maxUnits() << " | База: здоровье " << base.health() << "/" << base.maxHealth()
        << ", ресурсы " << base.resources() << ", юнитов базы " << base.unitCount() << "/" << base.maxUnits() << "\n";

    const std::vector<std::string>& log = Logger::lines();
    std::size_t first = log.size() > 3 ? log.size() - 3 : 0;
    for (std::size_t i = first; i < log.size(); ++i) std::cout << "  " << log[i] << "\n";

    if (!message.empty()) std::cout << ">> " << message << "\n";
}

int readInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            return value;
        }
        if (std::cin.eof()) std::exit(0);
        std::cin.clear();
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        std::cout << "Нужно ввести число.\n";
    }
}

int readIntInRange(const std::string& prompt, int lo, int hi) {
    while (true) {
        int v = readInt(prompt);
        if (v >= lo && v <= hi) return v;
        std::cout << "Введите число от " << lo << " до " << hi << ".\n";
    }
}

void waitEnter() {
    std::cout << "\nНажмите Enter, чтобы продолжить...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

void showText(const std::string& title, const std::vector<std::string>& lines) {
    clearScreen();
    std::cout << title << "\n\n";
    for (const std::string& line : lines) std::cout << line << "\n";
    waitEnter();
}

void showFrame(const GameController& game, const std::string& title) {
    clearScreen();
    std::cout << title << "\n\n";
    for (const std::string& line : renderMap(game)) std::cout << line << "\n";
    std::cout << "\nЮнитов: " << game.field().size() << "/" << game.field().maxUnits() << "\n";
    waitEnter();
}

void showBattleFrame(const GameController& game, const std::string& title) {
    clearScreen();
    std::cout << title << "\n\n";
    for (const std::string& line : renderMap(game)) std::cout << line << "\n";
    std::cout << "\nпрописные буквы - живые, строчные - погибшие\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(kFrameDelay));
}

void step(const GameController& game, const std::string& message) {
    drawScreen(game, message);
    waitEnter();
}

void printTypes() {
    std::cout << "\nВиды юнитов:\n";
    for (int i = 0; i < 10; ++i)
        std::cout << "  " << i + 1 << " - " << toString(kAllTypes[i]) << " [" << symbolOf(kAllTypes[i])
        << "], стоимость " << UnitTypeInfo::getInfo(kAllTypes[i])->cost << "\n";
}

std::string actAdd(GameController& game) {
    printTypes();
    int k = readInt("Вид (1-10): ");
    if (k < 1 || k > 10) return "Неверный номер вида юнита.";
    int x = readInt("x: ");
    int y = readInt("y: ");
    return game.createFreeUnit(kAllTypes[k - 1], x, y);
}

std::string actRemoveXY(GameController& game) {
    int x = readInt("x: ");
    int y = readInt("y: ");
    return game.removeUnitAt(x, y);
}

std::string actRemoveIndex(GameController& game) {
    std::cout << "Индекс ячейки = y * ширина + x (от 0 до "
        << game.map().width() * game.map().height() - 1 << ")\n";
    int idx = readInt("Индекс: ");
    return game.removeUnitByIndex(idx);
}

std::string actTeleport(GameController& game) {
    int x1 = readInt("Откуда x: ");
    int y1 = readInt("Откуда y: ");
    int x2 = readInt("Куда x: ");
    int y2 = readInt("Куда y: ");
    return game.teleportUnit(x1, y1, x2, y2);
}

std::string actInfo(const GameController& game) {
    int x = readInt("x: ");
    int y = readInt("y: ");
    const Unit* u = game.field().getUnit(x, y);
    if (!u) return "Там нет юнита.";

    std::vector<std::string> lines;
    lines.push_back(std::string("Вид: ") + toString(u->getType()) + " [" + unitSymbol(*u) + "], id " + std::to_string(u->id()));
    lines.push_back(std::string("Принадлежит: ") + (game.base().hasUnit(u->id()) ? "базе" : "никому (создан без базы)"));
    lines.push_back("Позиция: (" + std::to_string(u->x()) + "," + std::to_string(u->y()) + "), " + game.map().at(x, y).name());
    lines.push_back("Здоровье: " + std::to_string(u->health().value()) + "/" + std::to_string(u->info().maxHealth));
    lines.push_back("Броня: " + std::to_string(u->armor().value()));
    lines.push_back("Атака: " + std::to_string(u->attackValue()) + " (базовая " + std::to_string(u->info().baseAttack) + ")");
    lines.push_back("Очки движения: " + std::to_string(u->movePoints()).substr(0, 4) + " из " + std::to_string(u->info().baseSpeed).substr(0, 4));
    lines.push_back("Стоимость: " + std::to_string(u->info().cost));
    if (const Archer* a = dynamic_cast<const Archer*>(u))
        lines.push_back("Дальность: " + std::to_string(a->range()));
    if (const Mage* m = dynamic_cast<const Mage*>(u))
        lines.push_back("Мана: " + std::to_string(m->mana()));
    if (u->buffTurns() > 0)
        lines.push_back("Бафф действует ещё ходов: " + std::to_string(u->buffTurns()));
    showText("Информация о юните", lines);
    return "Информация показана.";
}

std::string actDamage(GameController& game) {
    int x = readInt("x: ");
    int y = readInt("y: ");
    int amount = readInt("Урон (до учёта брони): ");
    return game.damageUnit(x, y, amount);
}

std::string actAttack(GameController& game) {
    int x1 = readInt("Атакующий x: ");
    int y1 = readInt("Атакующий y: ");
    int x2 = readInt("Цель x: ");
    int y2 = readInt("Цель y: ");
    return game.attackUnit(x1, y1, x2, y2);
}

std::string actMassDamage(GameController& game) {
    int amount = readInt("Урон каждому юниту: ");
    return game.massDamage(amount);
}

std::string actRemoveDead(GameController& game) {
    return "Убрано погибших: " + std::to_string(game.removeDead()) + ".";
}

std::string actList(const GameController& game) {
    std::vector<std::string> lines;
    lines.push_back("Все юниты на поле (" + std::to_string(game.field().size()) + "):");
    int n = 0;
    for (const Unit& u : game.field()) {
        lines.push_back("  " + std::to_string(++n) + ". [" + unitSymbol(u) + "] " + toString(u.getType()) +
            " (" + std::to_string(u.x()) + "," + std::to_string(u.y()) + ") здоровье " +
            std::to_string(u.health().value()) + ", броня " + std::to_string(u.armor().value()) +
            ", атака " + std::to_string(u.attackValue()) + (game.base().hasUnit(u.id()) ? " [база]" : ""));
    }
    if (n == 0) lines.push_back("  (пусто)");
    showText("Список юнитов (обход итератором)", lines);
    return "Список показан.";
}

std::string actRandomArmy(GameController& game) {
    int requested = readInt("Сколько юнитов добавить: ");
    if (requested <= 0) return "Нужно положительное число.";

    const std::size_t before = game.field().size();

    std::vector<std::pair<int, int>> freeCells;
    for (int y = 0; y < game.map().height(); ++y)
        for (int x = 0; x < game.map().width(); ++x)
            if (game.map().at(x, y).symbol() == '*' && !game.field().getUnit(x, y) &&
                !(x == game.base().x() && y == game.base().y()))
                freeCells.push_back(std::make_pair(x, y));

    std::size_t room = game.field().maxUnits() > before ? game.field().maxUnits() - before : 0;
    std::size_t capacity = std::min(room, freeCells.size());

    for (std::size_t i = freeCells.size(); i > 1; --i)
        std::swap(freeCells[i - 1], freeCells[static_cast<std::size_t>(std::rand()) % i]);

    std::size_t toAdd = std::min(static_cast<std::size_t>(requested), capacity);
    for (std::size_t i = 0; i < toAdd; ++i)
        game.addFreeUnit(randomType(), freeCells[i].first, freeCells[i].second);

    std::string msg = "Добавлено юнитов: " + std::to_string(toAdd) +
        " из " + std::to_string(requested) + " (на равнину, без базы).";
    if (toAdd < static_cast<std::size_t>(requested)) {
        if (room <= freeCells.size())
            msg += " Достигнут лимит " + std::to_string(game.field().maxUnits()) +
            " (до этого на поле уже было " + std::to_string(before) + ").";
        else
            msg += " Свободная равнина закончилась.";
    }
    return msg;
}

std::string actClear(GameController& game) {
    game.clearField();
    return "Поле очищено.";
}

std::string actNewField(GameController& game) {
    int w = readIntInRange("Ширина поля (1-" + std::to_string(kMaxWidth) + "): ", 1, kMaxWidth);
    int h = readIntInRange("Высота поля (1-" + std::to_string(kMaxHeight) + "): ", 1, kMaxHeight);
    int m = readIntInRange("Максимум юнитов на поле (1-" + std::to_string(w * h) + "): ", 1, w * h);
    game.resetField(w, h, m);
    return "Создано новое поле " + std::to_string(w) + "x" + std::to_string(h) +
        ", лимит юнитов " + std::to_string(m) + ". Карта и база созданы заново.";
}

int blockRows(int count, int height, int maxCols) {
    int rows = 1;
    while (rows < height && (rows * rows < count || (count + rows - 1) / rows > maxCols)) ++rows;
    return rows;
}

int placeBlock(GameController& game, const std::vector<UnitType>& kinds, int count, int rows, int startX, int startY, std::vector<Unit*>& army) {
    int placed = 0;
    for (int x = startX; x < game.map().width() && placed < count; ++x) {
        for (int y = startY; y < startY + rows && placed < count; ++y) {
            UnitType t = kinds[std::rand() % kinds.size()];
            if (game.addFreeUnit(t, x, y)) {
                army.push_back(game.field().getUnit(x, y));
                ++placed;
            }
        }
    }
    return placed;
}

int buildTwoArmies(GameController& game, std::vector<Unit*>& left, std::vector<Unit*>& right) {
    const int gap = 2;
    int usableRows = game.map().height() - 1;
    int maxCols = (game.map().width() - gap) / 2;
    int count = std::min(static_cast<int>(game.field().maxUnits() / 2), maxCols * usableRows);
    if (count <= 0) return 0;

    int rows = blockRows(count, usableRows, maxCols);
    count = count / rows * rows;
    int cols = count / rows;
    int startX = (game.map().width() - (2 * cols + gap)) / 2;
    int startY = 1 + (usableRows - rows) / 2;

    std::vector<UnitType> leftKinds = { UnitType::Cataphract };
    std::vector<UnitType> rightKinds = { UnitType::Swordsman, UnitType::Spearman, UnitType::LightArcher, UnitType::HeavyArcher };
    placeBlock(game, leftKinds, count, rows, startX, startY, left);
    placeBlock(game, rightKinds, count, rows, startX + cols + gap, startY, right);
    return count;
}

int distanceBetween(const Unit& a, const Unit& b) {
    return std::max(std::abs(a.x() - b.x()), std::abs(a.y() - b.y()));
}

int countAlive(const std::vector<Unit*>& army) {
    int alive = 0;
    for (Unit* u : army)
        if (u->isAlive()) ++alive;
    return alive;
}

Unit* findNearestEnemy(const std::vector<std::vector<Unit*>>& armies, int own, const Unit& unit) {
    Unit* nearest = nullptr;
    int best = 1000000;
    for (int i = 0; i < static_cast<int>(armies.size()); ++i) {
        if (i == own) continue;
        for (Unit* e : armies[i]) {
            if (!e->isAlive()) continue;
            int d = distanceBetween(unit, *e);
            if (d < best) {
                best = d;
                nearest = e;
            }
        }
    }
    return nearest;
}

void stepToward(GameController& game, const Unit& unit, const Unit& target) {
    int dx = (target.x() > unit.x()) - (target.x() < unit.x());
    int dy = (target.y() > unit.y()) - (target.y() < unit.y());
    int x = unit.x();
    int y = unit.y();
    if (game.moveFree(x, y, x + dx, y + dy)) return;
    if (dx != 0 && game.moveFree(x, y, x + dx, y)) return;
    if (dy != 0 && game.moveFree(x, y, x, y + dy)) return;
}

bool othersAlive(const std::vector<std::vector<Unit*>>& armies, int winner) {
    for (int i = 0; i < static_cast<int>(armies.size()); ++i)
        if (i != winner && !armies[i].empty()) return true;
    return false;
}

void removeCasualties(GameController& game, std::vector<std::vector<Unit*>>& armies) {
    for (std::vector<Unit*>& army : armies)
        army.erase(std::remove_if(army.begin(), army.end(), [](Unit* u) { return !u->isAlive(); }), army.end());
    game.removeDead();
}

int battle(GameController& game, std::vector<std::vector<Unit*>>& armies, const std::vector<std::string>& names, int winner) {
    int round = 0;
    bool winnerLost = false;
    while (othersAlive(armies, winner) && round < 300) {
        ++round;

        for (int i = 0; i < static_cast<int>(armies.size()); ++i) {
            for (Unit* u : armies[i]) {
                Unit* enemy = findNearestEnemy(armies, i, *u);
                if (enemy && distanceBetween(*u, *enemy) > 1) stepToward(game, *u, *enemy);
            }
        }

        for (int i = 0; i < static_cast<int>(armies.size()); ++i) {
            for (Unit* u : armies[i]) {
                if (!u->isAlive()) continue;
                Unit* enemy = findNearestEnemy(armies, i, *u);
                if (!enemy || distanceBetween(*u, *enemy) > 1) continue;
                if (i == winner) {
                    if (countAlive(armies[i]) > 1 && (!winnerLost || std::rand() % 100 < 8)) {
                        u->takeDamage(1000000);
                        winnerLost = true;
                    }
                }
                else if (std::rand() % 100 < 30) {
                    u->takeDamage(1000000);
                }
            }
        }

        std::string title = "Раунд " + std::to_string(round) + ":";
        for (int i = 0; i < static_cast<int>(armies.size()); ++i)
            title += " " + names[i] + " - " + std::to_string(countAlive(armies[i])) + ";";
        showBattleFrame(game, title);
        removeCasualties(game, armies);
    }
    return round;
}

std::string scenarioNuke(GameController& game) {
    game.clearField();
    game.flattenArena();
    drawScreen(game, "Сценарий 1: поле очищено, местность выровнена. Сейчас две армии встанут друг напротив друга.");
    waitEnter();

    std::vector<Unit*> left;
    std::vector<Unit*> right;
    int count = buildTwoArmies(game, left, right);
    if (count == 0)
        return "Сценарий 1: поле слишком маленькое (нужна ширина от 4, высота от 2 и лимит от 2 юнитов).";

    drawScreen(game, "Шаг 1: две армии по " + std::to_string(count) + " юнитов стоят друг напротив друга.");
    waitEnter();

    for (Unit& u : game.field()) u.takeDamage(1000000);
    showFrame(game, "Шаг 2: ядерный удар. На поле остались одни трупы.");

    return "Сценарий 1 завершён: обе армии уничтожены одним ударом.";
}

std::string scenarioCataphractsWin(GameController& game) {
    game.clearField();
    game.flattenArena();
    drawScreen(game, "Сценарий 2: поле очищено, местность выровнена. Сейчас две армии встанут друг напротив друга.");
    waitEnter();

    std::vector<std::vector<Unit*>> armies(2);
    int count = buildTwoArmies(game, armies[0], armies[1]);
    if (count == 0)
        return "Сценарий 2: поле слишком маленькое (нужна ширина от 4, высота от 2 и лимит от 2 юнитов).";

    drawScreen(game, "Шаг 1: катафракты (слева) и пехота с лучниками (справа), по " + std::to_string(count) + " юнитов. Дальше бой пойдёт сам.");
    waitEnter();

    std::vector<std::string> names = { "Катафракты", "Пехота и лучники" };
    int rounds = battle(game, armies, names, 0);

    drawScreen(game, "Катафракты победили: выжило " + std::to_string(armies[0].size()) + " из " +
        std::to_string(count) + ", противников не осталось.");
    waitEnter();
    return "Сценарий 2 завершён: катафракты победили за " + std::to_string(rounds) + " раундов.";
}

std::string scenarioWorldWarByType(GameController& game) {
    game.clearField();
    game.flattenArena();
    drawScreen(game, "Сценарий 3: поле очищено, местность выровнена. Сейчас три армии встанут по углам поля.");
    waitEnter();

    int usableRows = game.map().height() - 1;
    int maxCols = game.map().width() / 3;
    int maxRows = usableRows / 3;
    int count = std::min(static_cast<int>(game.field().maxUnits() / 3), maxCols * maxRows);
    if (count <= 0)
        return "Сценарий 3: поле слишком маленькое (нужна ширина от 3, высота от 4 и лимит от 3 юнитов).";

    int rows = blockRows(count, maxRows, maxCols);
    count = count / rows * rows;
    int cols = count / rows;

    UnitType kinds[3] = { UnitType::Swordsman, UnitType::HeavyArcher, UnitType::Cataphract };
    std::vector<std::string> names = { "Пехота", "Лучники", "Конница" };
    std::vector<std::vector<Unit*>> armies(3);

    int startX[3] = { 0, game.map().width() - cols, 0 };
    int startY[3] = { 1, 1, game.map().height() - rows };
    for (int i = 0; i < 3; ++i)
        placeBlock(game, { kinds[i] }, count, rows, startX[i], startY[i], armies[i]);

    drawScreen(game, "Шаг 1: три армии по " + std::to_string(count) +
        " юнитов в углах поля: пехота, лучники и конница. Дальше война пойдёт сама.");
    waitEnter();

    int winner = std::rand() % 3;
    int rounds = battle(game, armies, names, winner);

    drawScreen(game, "Победа: " + names[winner] + ". Выжило " + std::to_string(armies[winner].size()) +
        " из " + std::to_string(count) + ".");
    waitEnter();
    return "Сценарий 3 завершён: победила армия \"" + names[winner] + "\" за " + std::to_string(rounds) + " раундов.";
}

std::string actCreateOnBase(GameController& game) {
    printTypes();
    int k = readInt("Вид (1-10): ");
    if (k < 1 || k > 10) return "Неверный номер вида юнита.";
    int x = readInt("x: ");
    int y = readInt("y: ");
    return game.createUnit(kAllTypes[k - 1], x, y);
}

std::string actStep(GameController& game) {
    int x1 = readInt("Откуда x: ");
    int y1 = readInt("Откуда y: ");
    int x2 = readInt("Куда x (соседняя клетка): ");
    int y2 = readInt("Куда y (соседняя клетка): ");
    return game.moveUnit(x1, y1, x2, y2);
}

std::string actProxy(const GameController& game) {
    printTypes();
    int k = readInt("Вид юнита (1-10): ");
    if (k < 1 || k > 10) return "Неверный номер вида юнита.";
    int x = readInt("x клетки: ");
    int y = readInt("y клетки: ");
    if (!game.map().inBounds(x, y)) return "Клетка вне поля.";

    UnitType type = kAllTypes[k - 1];
    const Terrain& terrain = game.map().at(x, y);
    std::vector<std::string> lines;
    lines.push_back("Клетка (" + std::to_string(x) + "," + std::to_string(y) + "): " + terrain.name());
    lines.push_back(std::string("Вид юнита: ") + toString(type));
    lines.push_back(std::string("Проходит: ") + (terrain.isPassable(type) ? "да" : "нет"));
    lines.push_back("Модификатор скорости: " + std::to_string(terrain.getSpeedModifier(type)).substr(0, 4));
    lines.push_back("Бонус защиты: " + std::to_string(terrain.getDefenseBonus(type)).substr(0, 4));
    lines.push_back("Шанс застрять: " + std::to_string(terrain.getSkipTurnChance(type)) + "%");
    showText("Свойства клетки (запрос идёт через TerrainProxy)", lines);
    return "Свойства клетки показаны.";
}

std::string actDamageBase(GameController& game) {
    int amount = readInt("Урон базе: ");
    return game.damageBase(amount);
}

std::string actBaseInfo(const GameController& game) {
    const Base& b = game.base();
    std::vector<std::string> lines;
    lines.push_back("Позиция: (" + std::to_string(b.x()) + "," + std::to_string(b.y()) + ")");
    lines.push_back("Здоровье: " + std::to_string(b.health()) + "/" + std::to_string(b.maxHealth()));
    lines.push_back("Уровень защиты: " + std::to_string(b.defenseLevel()));
    lines.push_back("Ресурсы: " + std::to_string(b.resources()));
    lines.push_back("Юнитов: " + std::to_string(b.unitCount()) + "/" + std::to_string(b.maxUnits()));
    lines.push_back(b.isDestroyed() ? "Состояние: база уничтожена" : "Состояние: база цела");
    int objects = 0;
    for (auto it = game.map().begin(); it != game.map().end(); ++it) ++objects;
    lines.push_back("Нейтральных объектов на карте: " + std::to_string(objects));
    lines.push_back("");
    lines.push_back("Живые юниты базы:");
    for (const Unit* u : b.units())
        lines.push_back("  #" + std::to_string(u->id()) + " " + toString(u->getType()) + " (" +
            std::to_string(u->x()) + "," + std::to_string(u->y()) + ") здоровье " + std::to_string(u->health().value()));
    if (b.units().empty()) lines.push_back("  (нет)");
    lines.push_back("");
    lines.push_back("Погибшие юниты (история):");
    for (const std::string& record : b.history()) lines.push_back("  " + record);
    if (b.history().empty()) lines.push_back("  (нет)");
    showText("Информация о базе", lines);
    return "Информация о базе показана.";
}

std::string actLog() {
    const std::vector<std::string>& log = Logger::lines();
    std::size_t first = log.size() > 25 ? log.size() - 25 : 0;
    std::vector<std::string> lines(log.begin() + static_cast<std::ptrdiff_t>(first), log.end());
    lines.push_back("");
    lines.push_back("Весь журнал также пишется в файл game.log");
    showText("Журнал событий (последние записи)", lines);
    return "Журнал показан.";
}

void buildArena(GameController& game) {
    game.newGame();
    game.map().clearObjects();
    for (int y = 0; y < 10; ++y)
        for (int x = 0; x < 8; ++x)
            game.map().setTerrain(x, y, std::make_unique<Plains>());
}

bool arenaFits(const GameController& game) {
    return game.map().width() >= 8 && game.map().height() >= 10 &&
        game.base().maxUnits() >= 6 && game.field().maxUnits() >= 6;
}

const std::string kNeedArena = "Для сценария нужно поле не меньше 8x10, лимит юнитов поля от 6 и лимит юнитов базы от 6.";

std::string scenarioBase(GameController& game) {
    game.newGame();
    step(game, "Сценарий 4: создана новая карта, база (B) стоит в левом верхнем углу.");

    step(game, game.createUnit(UnitType::Swordsman, 1, 0));
    step(game, game.createUnit(UnitType::LightArcher, 0, 1));
    step(game, game.createUnit(UnitType::Cataphract, 1, 1));
    step(game, game.createUnit(UnitType::Gryphon, 2, 1));
    step(game, game.createUnit(UnitType::Spearman, 0, 0));

    return "Сценарий 4 завершён: у базы " + std::to_string(game.base().resources()) + " ресурсов, юнитов " +
        std::to_string(game.base().unitCount()) + "/" + std::to_string(game.base().maxUnits()) + ".";
}

std::string scenarioTerrain(GameController& game) {
    if (!arenaFits(game)) return kNeedArena;
    buildArena(game);
    TerrainMap& map = game.map();
    map.setTerrain(3, 3, std::make_unique<Forest>());
    map.setTerrain(4, 3, std::make_unique<Swamp>());
    map.setTerrain(3, 4, std::make_unique<Forest>());
    map.setTerrain(3, 5, std::make_unique<Mountains>());
    map.setTerrain(3, 6, std::make_unique<Mountains>());
    map.setTerrain(4, 6, std::make_unique<Mountains>());

    game.createUnit(UnitType::Swordsman, 2, 3);
    game.createUnit(UnitType::Cataphract, 2, 4);
    game.createUnit(UnitType::Spearman, 2, 5);
    game.createUnit(UnitType::Gryphon, 2, 6);
    step(game, "Сценарий 5: полигон готов. Лес (&), болото (~), горы (^). Все ходы идут через TerrainProxy.");

    step(game, game.moveUnit(2, 3, 3, 3));
    step(game, game.damageUnit(3, 3, 30));
    step(game, game.moveUnit(3, 3, 4, 3));
    step(game, game.moveUnit(2, 4, 3, 4));
    step(game, game.moveUnit(2, 5, 3, 5));
    step(game, game.moveUnit(2, 6, 3, 6));
    step(game, game.moveUnit(3, 6, 4, 6));
    step(game, game.endTurn());
    step(game, game.moveUnit(3, 3, 4, 3));

    return "Сценарий 5 завершён: лес замедляет пехоту и защищает, тяжёлые в лес и пехота в горы не проходят, грифон летает над горами.";
}

std::string scenarioObjects(GameController& game) {
    if (!arenaFits(game)) return kNeedArena;
    buildArena(game);
    TerrainMap& map = game.map();
    map.placeObject(std::make_unique<TreasureChest>(3, 3, game.base(), 50));
    map.placeObject(std::make_unique<Trap>(3, 4, 30));
    map.placeObject(std::make_unique<Altar>(3, 5));
    map.placeObject(std::make_unique<Altar>(3, 6));
    map.placeObject(std::make_unique<Altar>(3, 7));
    map.placeObject(std::make_unique<EnergySource>(3, 8));

    game.createUnit(UnitType::Swordsman, 2, 3);
    game.createUnit(UnitType::Spearman, 2, 4);
    game.createUnit(UnitType::Swordsman, 2, 5);
    game.createUnit(UnitType::LightArcher, 2, 6);
    game.createUnit(UnitType::FireMage, 2, 7);
    game.createUnit(UnitType::Swordsman, 2, 8);
    step(game, "Сценарий 6: рядом с юнитами стоят объекты: C сундук, T ловушка, три алтаря P, R источник энергии.");

    step(game, game.moveUnit(2, 3, 3, 3));
    step(game, game.moveUnit(2, 4, 3, 4));
    step(game, game.moveUnit(2, 5, 3, 5));
    step(game, game.moveUnit(2, 6, 3, 6));
    step(game, game.moveUnit(2, 7, 3, 7));
    step(game, game.damageUnit(2, 8, 70));
    step(game, game.moveUnit(2, 8, 3, 8));

    return "Сценарий 6 завершён: unit + chest, unit - trap, unit * altar, unit / source (у алтаря три разные стратегии).";
}

std::string scenarioObserver(GameController& game) {
    game.newGame();
    game.createUnit(UnitType::Swordsman, 1, 0);
    game.createUnit(UnitType::Spearman, 0, 1);
    step(game, "Сценарий 7: на базе два юнита. База подписана на события каждого из них (Observer).");

    step(game, game.damageUnit(1, 0, 40));
    step(game, game.damageUnit(1, 0, 500));
    step(game, "Юнит погиб (строчная m), база получила Destroyed: счётчик уменьшился, юнит попал в историю.");
    step(game, game.endTurn());
    step(game, game.damageBase(40));

    return "Сценарий 7 завершён: погибших в истории базы " + std::to_string(game.base().history().size()) + ".";
}

void check(std::vector<std::string>& report, int& failed, const std::string& name, bool ok) {
    report.push_back(std::string(ok ? "[OK] " : "[ПРОВАЛ] ") + name);
    if (!ok) ++failed;
}

std::string runTests() {
    std::vector<std::string> report;
    int failed = 0;

    TerrainProxy mountains(std::make_unique<Mountains>());
    TerrainProxy forest(std::make_unique<Forest>());
    TerrainProxy swamp(std::make_unique<Swamp>());
    TerrainProxy plains(std::make_unique<Plains>());

    check(report, failed, "Горы непроходимы для нелетающих", !mountains.isPassable(UnitType::Swordsman) && !mountains.isPassable(UnitType::Cataphract));
    check(report, failed, "Горы проходимы для летающих", mountains.isPassable(UnitType::Gryphon) && mountains.isPassable(UnitType::Wyvern));
    check(report, failed, "Лес: тяжёлые юниты не проходят", !forest.isPassable(UnitType::Cataphract) && !forest.isPassable(UnitType::HeavyArcher));
    check(report, failed, "Лес: пехота проходит и получает бонус защиты", forest.isPassable(UnitType::Swordsman) && forest.getDefenseBonus(UnitType::Swordsman) > 0);
    check(report, failed, "Лес замедляет пехоту", forest.getSpeedModifier(UnitType::Spearman) == 0.5);
    check(report, failed, "Болото замедляет всех нелетающих", swamp.getSpeedModifier(UnitType::Swordsman) == 0.5 && swamp.getSpeedModifier(UnitType::Cataphract) == 0.5);
    check(report, failed, "Болото: летающие не замедляются (проверка прокси)", swamp.getSpeedModifier(UnitType::Gryphon) == 1.0);
    check(report, failed, "Болото: нелетающие могут застрять", swamp.getSkipTurnChance(UnitType::Swordsman) > 0);
    check(report, failed, "Равнина без ограничений", plains.isPassable(UnitType::Cataphract) && plains.getSpeedModifier(UnitType::Cataphract) == 1.0);

    GameController game(20, 20, 400, 3);
    Unit* unit = game.base().createUnit(UnitType::Swordsman, 1, 0);
    check(report, failed, "База учитывает созданного юнита", unit != nullptr && game.base().unitCount() == 1);
    if (unit) unit->takeDamage(100000);
    check(report, failed, "При гибели юнита счётчик базы уменьшается (Observer)", game.base().unitCount() == 0);
    check(report, failed, "Погибший юнит попал в историю базы", game.base().history().size() == 1);

    game.removeDead();
    game.base().createUnit(UnitType::Swordsman, 1, 0);
    game.base().createUnit(UnitType::Spearman, 0, 1);
    game.base().createUnit(UnitType::LightArcher, 1, 1);
    check(report, failed, "Лимит юнитов базы соблюдается", game.base().createUnit(UnitType::Spearman, 2, 1) == nullptr);

    Swordsman s(5, 5);
    TreasureChest chest(5, 6, game.base(), 50);
    int resources = game.base().resources();
    s + chest;
    check(report, failed, "unit + chest: ресурсы базы выросли", game.base().resources() == resources + 50);

    Trap trap(5, 7, 30);
    int hp = s.health().value();
    s - trap;
    check(report, failed, "unit - trap: юнит получил урон", s.health().value() < hp);

    Altar altar(5, 8);
    int attack = s.attackValue();
    s * altar;
    check(report, failed, "unit * altar: пехотинец получил +атаку", s.attackValue() > attack);
    LightArcher archer(6, 6);
    int range = archer.range();
    archer * altar;
    check(report, failed, "unit * altar: лучник получил +дальность", archer.range() > range);
    FireMage mage(6, 7);
    int mana = mage.mana();
    mage * altar;
    check(report, failed, "unit * altar: маг получил +ману", mage.mana() > mana);

    EnergySource source(5, 9);
    s / source;
    check(report, failed, "unit / source: здоровье восстановлено полностью", s.health().value() == s.info().maxHealth);

    int objects = 0;
    for (auto it = game.map().begin(); it != game.map().end(); ++it) ++objects;
    check(report, failed, "На карте 4 нейтральных объекта (обход итератором)", objects == 4);

    GameController copyGame(10, 10, 100, 6);
    copyGame.field().clear();
    copyGame.addFreeUnit(UnitType::Swordsman, 4, 4);
    GameField copy = copyGame.field();
    copy.getUnit(4, 4)->takeDamage(1000);
    check(report, failed, "Глубокая копия поля независима от оригинала (ЛР 1)", copyGame.field().getUnit(4, 4)->isAlive());
    GameField moved = std::move(copy);
    check(report, failed, "Перемещение поля опустошает источник (ЛР 1)", copy.size() == 0 && moved.size() == 1);

    report.push_back("");
    report.push_back("Пройдено проверок: " + std::to_string(report.size() - 1 - static_cast<std::size_t>(failed)) +
        ", провалено: " + std::to_string(failed));
    showText("Автотесты", report);
    return failed == 0 ? "Все автотесты пройдены." : "Есть проваленные автотесты.";
}

int main() {
    setlocale(LC_ALL, "rus");
    enableColors();
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::cout << "Установите параметры игры:\n";
    int w = readIntInRange("Ширина поля (1-" + std::to_string(kMaxWidth) + "): ", 1, kMaxWidth);
    int h = readIntInRange("Высота поля (1-" + std::to_string(kMaxHeight) + "): ", 1, kMaxHeight);
    int m = readIntInRange("Максимум юнитов на поле (1-" + std::to_string(w * h) + "): ", 1, w * h);
    int b = readIntInRange("Лимит юнитов базы (6-50): ", 6, 50);

    GameController game(w, h, m, b);
    std::string message = "Добро пожаловать! База (B) в левом верхнем углу. Меню ЛР 1 и ЛР 2 переключаются пунктом 99.";

    while (true) {
        drawScreen(game, message);
        int choice = readInt("\nВыбор: ");
        message.clear();

        if (choice >= 1 && choice <= 16) menuPage = 0;
        else if (choice >= 17 && choice <= 28) menuPage = 1;

        try {
            switch (choice) {
            case 0: return 0;
            case 1: message = actAdd(game); break;
            case 2: message = actRemoveXY(game); break;
            case 3: message = actRemoveIndex(game); break;
            case 4: message = actTeleport(game); break;
            case 5: message = actInfo(game); break;
            case 6: message = actDamage(game); break;
            case 7: message = actAttack(game); break;
            case 8: message = actMassDamage(game); break;
            case 9: message = actRemoveDead(game); break;
            case 10: message = actList(game); break;
            case 11: message = actRandomArmy(game); break;
            case 12: message = actClear(game); break;
            case 13: message = scenarioNuke(game); break;
            case 14: message = scenarioCataphractsWin(game); break;
            case 15: message = scenarioWorldWarByType(game); break;
            case 16: message = actNewField(game); break;
            case 17: message = actCreateOnBase(game); break;
            case 18: message = actStep(game); break;
            case 19: message = game.endTurn(); break;
            case 20: message = actProxy(game); break;
            case 21: message = actDamageBase(game); break;
            case 22: message = actBaseInfo(game); break;
            case 23: message = actLog(); break;
            case 24: message = scenarioBase(game); break;
            case 25: message = scenarioTerrain(game); break;
            case 26: message = scenarioObjects(game); break;
            case 27: message = scenarioObserver(game); break;
            case 28: message = runTests(); break;
            case 99: menuPage = 1 - menuPage; break;
            default: message = "Нет такого пункта меню.";
            }
        }
        catch (const std::exception& e) {
            message = std::string("Ошибка: ") + e.what();
        }
    }
}
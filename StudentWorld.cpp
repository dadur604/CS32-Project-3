//#define DEBUG_DIRTS 5

#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_pitDone = false;
    m_aliveBugs = 0;
}

int StudentWorld::init()
{
    // Add Socrates
    m_socrates = new Socrates(this);

    int level = getLevel();

    struct Point {
        int x;
        int y;
    };

    // Add pits
    for (int i = 0; i < level; i++) {
        int x, y;
        randomPointInCircle(true, x, y);
        // we have a valid x,y!

        m_actors.push_back(new Pit(x, y, this));
    }

    // Add food
    int food = min(5 * level, 25);
    for (int i = 0; i < food; i++) {
        int x, y;
        randomPointInCircle(true, x, y);
        // we have a valid x,y!

        m_actors.push_back(new Food(x, y, this));
    }

    // Add dirts
    int dirts =
#ifdef DEBUG_DIRTS 
        DEBUG_DIRTS;
#else
        max(180 - 20 * level, 20);
#endif
    for (int i = 0; i < dirts; i++) {
        // Get random x,y pos of dirt (in relation to circle origin)
        int x, y;
        randomPointInCircle(true, x, y);

        m_actors.push_back(new Dirt(x, y, this));
    }

    m_pitDone = false;
    m_aliveBugs = 0;

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // Update Socrates
    int size = m_actors.size();
    m_socrates->doSomething();

    // Update remaining Actors
    list<Actor*>::iterator it = m_actors.begin();
    for (int i = 0; i < size + 1; i++) {
        if (m_pitDone && m_aliveBugs == 0)
            return GWSTATUS_FINISHED_LEVEL;
        if (!m_socrates->Alive()) {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }

        if (i == size) break;

        (*it)->doSomething();

        it++;
    }

    // Remove dead actors
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ) {
        if (!(*it)->Alive()) {
            delete *it;
            auto toErase = it;
            it++;
            m_actors.erase(toErase);
        }
        else {
            it++;
        }
    }

    // Update game text
    ostringstream ss;
    ss  << "Score: " << setw(6) << setfill('0') << internal << getScore()
        << "  Level: " << getLevel()
        << "  Lives: " << getLives()
        << "  Health: "<< m_socrates->Health()
        << "  Sprays: " << m_socrates->SpraysRemaining()
        << "  Flames: " << m_socrates->FlamesRemaining();

    setGameStatText(ss.str());

    // Add fungi
    int chanceFungus = max(510 - getLevel() * 10, 200);
    if (randInt(0, chanceFungus - 1) == 0) {
        int x, y;
        randomPointAroundCircle(x, y);

        int life = max(randInt(0, 300 - 10 * getLevel() - 1), 50);

        Fungus* fungus = new Fungus(x, y, life, this);
        addActor(fungus);
    }

    // Add goodies
    int chanceGoodie = max(510 - getLevel() * 10, 250);
    if (randInt(0, chanceGoodie - 1) == 0) {
        int x, y;
        randomPointAroundCircle(x, y);
        
        Pickup* goodie;

        int life = max(randInt(0, 300 - 10 * getLevel() - 1), 50);

        int choice = randInt(0, 9);
        if (choice < 6) {
            // HEALTH
            goodie = new HealthGoodie(x, y, life, this);
        }
        else if (choice < 9) {
            // FLAME
            goodie = new FlameGoodie(x, y, life, this);
        }
        else {
            // LIFE
            goodie = new LifeGoodie(x, y, life, this);
        }

        addActor(goodie);
    }

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_socrates;
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end();) {
        delete *it;
        auto toErase = it;
        it++;
        m_actors.erase(toErase);
    }
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

void StudentWorld::addActor(Actor* actor) {
    m_actors.push_back(actor);
}

void StudentWorld::removeActor(Actor* actor) {
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ) {
        if (*it == actor) {
            delete *it;
            auto toErase = it;
            it++;
            m_actors.erase(toErase);
            break;
        }
        it++;
    }
}

bool StudentWorld::getOverlapObject(double x, double y, Actor*& object, bool (*predicate)(Actor*), double range) {
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if (!(*it)->Alive()) continue;
        if (!predicate(*it)) continue;

        double otherX = (*it)->getX();
        double otherY = (*it)->getY();

        if (euclideanDistance(x, y, otherX, otherY) <= range) {
            object = *it;
            return true;
        }
    }

    return false;
}

bool StudentWorld::getOverlapObject(const Actor* src, Actor*& object, bool (*predicate)(Actor*), double range) {
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if (*it == src) continue;
        if (!(*it)->Alive()) continue;
        if (!predicate(*it)) continue;
        double otherX = (*it)->getX();
        double otherY = (*it)->getY();

        double dist = euclideanDistance(src->getX(), src->getY(), otherX, otherY);
        if (dist <= range) {
            object = *it;
            return true;
        }
    }

    return false;
}

bool StudentWorld::getClosestObject(const Actor* src, Actor*& object, bool (*predicate)(Actor*), double range) {
    double closestDist;
    Actor* closestObj;
    bool first = true;
    
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if (*it == src) continue;
        if (!(*it)->Alive()) continue;
        if (!predicate(*it)) continue;
        double otherX = (*it)->getX();
        double otherY = (*it)->getY();

        double dist = euclideanDistance(src->getX(), src->getY(), otherX, otherY);
        if (dist > range) continue;
        if (first) {
            closestDist = dist;
            closestObj = *it;
            first = false;
        }
        else {
            if (dist < closestDist) {
                closestDist = dist;
                closestObj = *it;
            }
        }
    }

    if (first) return false; // didn't find any

    object = closestObj;
    return true;
}

bool StudentWorld::getOverlapSocrates(double x, double y, Socrates*& socrates, double range) {
    socrates = m_socrates;
    return euclideanDistance(m_socrates->getX(), m_socrates->getY(), x, y) <= range;
}

bool StudentWorld::getOverlapSocrates(const Actor* src, Socrates*& socrates, double range) {
    socrates = m_socrates;
    return getOverlapSocrates(src->getX(), src->getY(), socrates, range);
}

void StudentWorld::randomPointInCircle(bool detectOverlaps, int& x, int& y) {
    int _x, _y;
    Actor* obj;
    do {
        _x = randInt(-120, 120);
        _y = randInt(-1 * sqrt(pow(120, 2) - pow(_x, 2)), sqrt(pow(120, 2) - pow(_x, 2)));
    } while (detectOverlaps &&
        getOverlapObject(_x + VIEW_WIDTH / 2, _y + VIEW_HEIGHT / 2, obj, [](Actor* a) { return a->blockPlacement(); }));

    x = _x + (VIEW_WIDTH / 2);
    y = _y + (VIEW_HEIGHT / 2);
}

void StudentWorld::randomPointAroundCircle(int& x, int& y) {
    int theta = randInt(0, 359);
    x = VIEW_RADIUS * cos(theta) + VIEW_WIDTH / 2;
    y = VIEW_RADIUS * sin(theta) + VIEW_HEIGHT / 2;
}
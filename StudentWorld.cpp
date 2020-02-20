//#define DEBUG_DIRTS 5

#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
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

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    int size = m_actors.size();
    m_socrates->doSomething();

    list<Actor*>::iterator it = m_actors.begin();
    for (int i = 0; i < size; i++) {
        (*it)->doSomething();
        it++;
    }

    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ) {
        if (!(*it)->Alive()) {
            delete* it;
            auto toErase = it;
            it++;
            m_actors.erase(toErase);
        }
        else {
            it++;
        }
    }

    ostringstream ss;
    ss  << "Score: " << getScore()
        << "  Level: " << getLevel()
        << "  Lives: " << getLives()
        << "  Health: "<< m_socrates->Health()
        << "  Sprays: " << m_socrates->SpraysRemaining()
        << "  Flames: " << m_socrates->FlamesRemaining();

    setGameStatText(ss.str());

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_socrates;
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        delete *it;
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

bool StudentWorld::getOverlapObject(int x, int y, Actor*& object, bool (*predicate)(Actor*)) {
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if (!(*it)->Alive()) continue;
        if (!predicate(*it)) continue;

        double otherX = (*it)->getX();
        double otherY = (*it)->getY();

        cout << "getOverlapObject called\n";
        cout << "distance: " << euclideanDistance(x, y, otherX, otherY) << endl;
        if (euclideanDistance(x, y, otherX, otherY) <= SPRITE_RADIUS * 2) {
            object = *it;
            cout << "overlap found!\n";
            return true;
        }
    }

    return false;
}

bool StudentWorld::getOverlapObject(const Actor* src, Actor*& object, bool (*predicate)(Actor*)) {
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if (*it == src) continue;
        if (!(*it)->Alive()) continue;
        if (!predicate(*it)) continue;
        double otherX = (*it)->getX();
        double otherY = (*it)->getY();

        if (euclideanDistance(src->getX(), src->getY(), otherX, otherY) <= SPRITE_RADIUS * 2) {
            object = *it;
            return true;
        }
    }

    return false;
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
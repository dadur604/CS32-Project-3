//#define DEBUG_DIRTS 5

#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
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

    // Add dirts
    int level = getLevel();
    int dirts =
#ifdef DEBUG_DIRTS 
        DEBUG_DIRTS;
#else
        max(180 - 20 * level, 20);
#endif
    for (int i = 0; i < dirts; i++) {
        // Get random x,y pos of dirt (in relation to circle origin)
        int x = randInt(-120, 120);
        int y = randInt(-1 * sqrt(pow(120, 2) - pow(x, 2)), sqrt(pow(120, 2) - pow(x, 2)));

        m_actors.push_back(new Dirt(x + VIEW_WIDTH / 2, y + VIEW_HEIGHT / 2, this));
    }

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    m_socrates->doSomething();
    for (int i = 0; i < m_actors.size(); i++) {
        m_actors[i]->doSomething();
    }

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_socrates;
    for (int i = 0; i < m_actors.size(); i++) {
        delete m_actors[i];
    }
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

void StudentWorld::addActor(Actor* actor) {
    m_actors.push_back(actor);
}

void StudentWorld::removeActor(Actor* actor) {
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if (*it == actor) {
            delete *it;
            m_actors.erase(it);
            break;
        }
    }
}

bool StudentWorld::getOverlapObject(const Actor* src, Actor*& object, bool (*predicate)(Actor*)) {
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if (*it == src) continue;
        if (!predicate(*it)) continue;
        double otherX = (*it)->getX();
        double otherY = (*it)->getY();

        if (sqrt(pow(src->getX() - otherX, 2) + pow(src->getY() - otherY, 2)) <= SPRITE_RADIUS * 2) {
            object = *it;
            return true;
        }
    }

    return false;
}
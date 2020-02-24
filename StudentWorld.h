#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

// HELPER

double inline degToRad(double degrees) {
    return degrees / 180 * 3.1415;
}

double inline radToDeg(double radians) {
    return radians / 3.1415 * 180;
}

double inline euclideanDistance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

// where x2, y2 is the TARGET
Direction inline dirTowards(double x1, double y1, double x2, double y2) {
    double dy = y2 - y1;
    double dx = x2 - x1;

    double dir = radToDeg(atan2(dy, dx));
    return dir;
}

//

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();

    void addActor(Actor* actor);

    void pitDone() {
        m_pitDone = true;
    }
    void updateBugCount(int count) {
        m_aliveBugs += count;
    }

    bool getOverlapObject(const Actor* src, Actor*& object, bool (*predicate)(Actor*), double range = SPRITE_RADIUS * 2);
    bool getOverlapObject(double x, double y, Actor*& object, bool (*predicate)(Actor*), double range = SPRITE_RADIUS * 2);
    bool getOverlapSocrates(double x, double y, Socrates*& socrates, double range = SPRITE_WIDTH);
    bool getOverlapSocrates(const Actor* src, Socrates*& socrates, double range = SPRITE_WIDTH);

    bool getClosestObject(const Actor* src, Actor*& object, bool (*predicate)(Actor*), double range);

    void randomPointInCircle(bool detectOverlaps, int& x, int& y);
    void randomPointAroundCircle(int& x, int& y);

private:
    std::list<Actor*> m_actors;
    Socrates* m_socrates;
    void removeActor(Actor* actor);

    bool m_pitDone;
    int m_aliveBugs;
};

#endif // STUDENTWORLD_H_

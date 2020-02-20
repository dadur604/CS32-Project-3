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

double inline euclideanDistance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
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
    void removeActor(Actor* actor);

    bool getOverlapObject(const Actor* src, Actor*& object, bool (*predicate)(Actor*));
    bool getOverlapObject(int x, int y, Actor*& object, bool (*predicate)(Actor*));

    void randomPointInCircle(bool detectOverlaps, int& x, int& y);

private:
    std::list<Actor*> m_actors;
    Socrates* m_socrates;
};

#endif // STUDENTWORLD_H_

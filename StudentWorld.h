#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

// HELPER

double inline degToRad(double degrees) {
    return degrees / 180 * 3.1415;
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

private:
    std::vector<Actor*> m_actors;
    Socrates* m_socrates;
};

#endif // STUDENTWORLD_H_

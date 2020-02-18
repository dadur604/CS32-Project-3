#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject {

public:
	Actor(int imageId, double startX, double startY, Direction dir, int depth, double size, StudentWorld* world);

	virtual void doSomething() = 0;

	bool Alive();
	int Health();
	void updateHealth(int health);

	StudentWorld* getWorld();

protected:
	void setHealth(int health);

private:
	int m_health;
	StudentWorld* m_world;
};

class Dirt : public Actor {

public:
	Dirt(double startX, double startY, StudentWorld* world);

	virtual void doSomething();
};

class Socrates : public Actor {
	const int MAX_SPRAYS = 20;
	const int START_FLAMES = 5;
public:
	Socrates(StudentWorld* world);

	virtual void doSomething();

private:
	int m_flamesRemaining;
	int m_spraysRemaining;

	void moveAround(int degrees);

	double posAngle;
};

#endif // ACTOR_H_

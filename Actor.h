#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject {

public:
	Actor(int imageId, double startX, double startY, Direction dir, int depth, double size, StudentWorld* world);

	void doSomething();

	bool Alive();
	int Health();
	void updateHealth(int health);

	virtual bool isDamageable() { return false; }

	StudentWorld* getWorld();

protected:
	void setHealth(int health);

private:
	int m_health;
	StudentWorld* m_world;

	virtual void update() = 0;
};

class Dirt : public Actor {

public:
	Dirt(double startX, double startY, StudentWorld* world);

	virtual bool isDamageable() { return true; }

	virtual void update();
};

class Socrates : public Actor {
	const int MAX_SPRAYS = 20;
	const int START_FLAMES = 5;
public:
	Socrates(StudentWorld* world);

	virtual bool isDamageable() { return true; }

	virtual void update();

private:
	int m_flamesRemaining;
	int m_spraysRemaining;

	void moveAround(int degrees);
	void spray();
	void flame();

	double posAngle;
};

class Projectile : public Actor {
public:
	Projectile(double startX, double startY, Direction dir, int speed, int damage, int ImageID, StudentWorld* world);

	virtual bool isMaxDistance() = 0;
	virtual void update();

protected:
	int getStartX() {
		return m_startX;
	}

	int getStartY() {
		return m_startY;
	}

private:
	int m_startX;
	int m_startY;
	int m_speed;
	int m_damage;
};

class Spray : public Projectile {
public:
	Spray(double startX, double startY, Direction dir, StudentWorld* world);
	
	virtual bool isMaxDistance();
};

class Flame : public Projectile {
public:
	Flame(double startX, double startY, Direction dir, StudentWorld* world);

	virtual bool isMaxDistance();
};

#endif // ACTOR_H_

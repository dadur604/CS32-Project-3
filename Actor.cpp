#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// ACTOR

Actor::Actor(int imageId, double startX, double startY, Direction dir, int depth, double size, StudentWorld* world)
	: GraphObject(imageId, startX, startY, dir, depth, size) {
	m_world = world;
}

bool Actor::Alive() {
	return m_health > 0;
}

int Actor::Health() {
	return m_health;
}

void Actor::updateHealth(int health) {
	m_health += health;
	if (m_health <= 0) {
		// on death
	}
}

StudentWorld* Actor::getWorld() {
	return m_world;
}

void Actor::setHealth(int health) {
	m_health = health;
}

// DIRT

Dirt::Dirt(double startX, double startY, StudentWorld* world)
	: Actor(IID_DIRT, startX, startY, 0, 1, 1, world) {
	setHealth(1);
}

void Dirt::doSomething() {
	return;
}

// SOCRATES	

Socrates::Socrates(StudentWorld* world)
	: Actor(IID_PLAYER, 0, 128, 0, 1, 1, world) {
	setHealth(100);
	m_spraysRemaining = MAX_SPRAYS;
	m_flamesRemaining = START_FLAMES;
	posAngle = 180;
}

void Socrates::doSomething() {
	if (!Alive()) return;

	int key;
	if (getWorld()->getKey(key)) {

		switch (key) {
			case KEY_PRESS_LEFT:
				moveAround(5);
				break;
			case KEY_PRESS_RIGHT:
				moveAround(-5);
				break;
		}
	}
}

void Socrates::moveAround(int degrees) {
	double r = VIEW_DIAMETER;
	double xHalf = VIEW_WIDTH / 2;
	double yHalf = VIEW_HEIGHT / 2;
	double rad_posAngle;

	posAngle += degrees;
	if (posAngle >= 360) posAngle -= 360;
	rad_posAngle = degToRad(posAngle);
	moveTo(r * cos(rad_posAngle) + xHalf, r * sin(rad_posAngle) + yHalf);

	double lookDir = posAngle - 180;
	if (lookDir < 0) lookDir += 360;
	setDirection(lookDir);
}
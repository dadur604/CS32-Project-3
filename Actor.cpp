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
		// Will be deleted by StudentWorld
	}
}

StudentWorld* Actor::getWorld() {
	return m_world;
}

void Actor::setHealth(int health) {
	m_health = health;
}

void Actor::doSomething() {
	if (!Alive()) return;
	update();
}

// DIRT

Dirt::Dirt(double startX, double startY, StudentWorld* world)
	: Actor(IID_DIRT, startX, startY, 0, 1, 1, world) {
	setHealth(1);
}

void Dirt::update() {
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

void Socrates::update() {
	int key;
	if (getWorld()->getKey(key)) {

		switch (key) {
			case KEY_PRESS_LEFT:
				moveAround(5);
				break;
			case KEY_PRESS_RIGHT:
				moveAround(-5);
				break;
			case KEY_PRESS_SPACE:
				spray();
				break;
			case KEY_PRESS_ENTER:
				flame();
				break;
		}
	}
	else {
		// No key pressed
		if (m_spraysRemaining < MAX_SPRAYS) m_spraysRemaining++;
	}
}

void Socrates::spray() {
	if (m_spraysRemaining > 0) {
		double x, y;
		getPositionInThisDirection(getDirection(), 2 * SPRITE_RADIUS, x, y);
		getWorld()->addActor(new Spray(x, y, getDirection(), getWorld()));
		getWorld()->playSound(SOUND_PLAYER_SPRAY);
		m_spraysRemaining--;
	}
}

void Socrates::flame() {
	if (m_flamesRemaining > 0) {
		Direction direction = getDirection();
		for (int i = 0; i < 16; i++) {
			static double x, y;
			Direction newDir = direction + i * 22;
			getPositionInThisDirection(newDir, 2 * SPRITE_RADIUS, x, y);
			getWorld()->addActor(new Flame(x, y, newDir, getWorld()));
		}
		getWorld()->playSound(SOUND_PLAYER_SPRAY);
		m_flamesRemaining--;
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

// PROJECTILE

Projectile::Projectile(double startX, double startY, Direction dir, int speed, int damage, int imageID, StudentWorld* world)
	: Actor(imageID, startX, startY, dir, 1, 1, world) {
	m_startX = startX;
	m_startY = startY;
	m_speed = speed;
	m_damage = damage;
}

void Projectile::update() {
	Actor* object = nullptr;
	if (getWorld()->getOverlapObject(this, object, [](Actor* a) {return a->isDamageable();})) {
		// do damage
		object->updateHealth(-1 * m_damage);
		// remove ourselves
		setHealth(0);
		return;
	}

	// move
	moveAngle(getDirection(), m_speed);

	if (isMaxDistance()) {
		setHealth(0);
	}
}

// SPRAY

Spray::Spray(double startX, double startY, Direction dir, StudentWorld* world)
	: Projectile(startX, startY, dir, SPRITE_RADIUS*2, 2, IID_SPRAY, world) {
	setHealth(1);
}

bool Spray::isMaxDistance() {
	if (sqrt(pow(getStartX() - getX(), 2) + pow(getStartY() - getY(), 2)) >= 112) return true;
	return false;
}

// FLAME

Flame::Flame(double startX, double startY, Direction dir, StudentWorld* world)
	: Projectile(startX, startY, dir, SPRITE_RADIUS * 2, 2, IID_FLAME, world) {
	setHealth(1);
}

bool Flame::isMaxDistance() {
	if (sqrt(pow(getStartX() - getX(), 2) + pow(getStartY() - getY(), 2)) >= 32) return true;
	return false;
}
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

// FOOD

Food::Food(double startX, double startY, StudentWorld* world)
	: Actor(IID_FOOD, startX, startY, 90, 1, 1, world) {
	setHealth(1);
}

void Food::update() {
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

void Socrates::updateHealth(int health) {
	Actor::updateHealth(health);
	if (Alive())
		getWorld()->playSound(SOUND_PLAYER_HURT);
	else
		getWorld()->playSound(SOUND_PLAYER_DIE);
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
	double r = VIEW_RADIUS;
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
	if (euclideanDistance(getStartX(), getStartY(), getX(), getY()) >= 112) return true;
	return false;
}

// FLAME

Flame::Flame(double startX, double startY, Direction dir, StudentWorld* world)
	: Projectile(startX, startY, dir, SPRITE_RADIUS * 2, 2, IID_FLAME, world) {
	setHealth(1);
}

bool Flame::isMaxDistance() {
	if (euclideanDistance(getStartX(), getStartY(), getX(), getY()) >= 32) return true;
	return false;
}

// PIT

Pit::Pit(double startX, double startY, StudentWorld* world)
	: Actor(IID_PIT, startX, startY, 0, 1, 1, world) {
	m_regSalmonella = 5;
	m_aggSalmonella = 3;
	m_ecoli = 0;
	setHealth(1);
}

void Pit::update() {
	if (m_regSalmonella + m_aggSalmonella + m_ecoli == 0) {
		getWorld()->pitDone();
		setHealth(0);
		return;
	}

	if (randInt(1, 50) == 50) {
		Bacteria* bacterium;
		int startX = getX();
		int startY = getY();
	again:
		int choice = randInt(0, 2);
		switch (choice) {
			case 0: // REG SALM
				if (m_regSalmonella <= 0) goto again;
				bacterium = new Salmonella(startX, startY, getWorld());
				m_regSalmonella--;
				break;
			case 1: // AGG SALM
				if (m_aggSalmonella <= 0) goto again;
				bacterium = new AggSalmonella(startX, startY, getWorld());
				m_regSalmonella--;
				break;
			case 2: // ECOLI
				if (m_ecoli <= 0) goto again;
				bacterium = new EColi(startX, startY, getWorld());
				m_ecoli--;
				break;
		}

		getWorld()->addActor(bacterium);
		getWorld()->playSound(SOUND_BACTERIUM_BORN);
	}
}

// DIRTY DIRTY BUGSSSS //

Bacteria::Bacteria(int imageId, int startX, int startY, StudentWorld* world)
	: Actor(imageId, startX, startY, 90, 0, 1, world) {
	m_foodSinceLast = 0;
	world->updateBugCount(1);
}

template<typename T>
void Bacteria::duplicate() {
	double newX = getX();
	double newY = getY();

	if (getX() < VIEW_WIDTH / 2)
		newX += SPRITE_WIDTH / 2;
	else if (getX() > VIEW_WIDTH / 2)
		newX -= SPRITE_WIDTH / 2;

	if (getY() < VIEW_HEIGHT / 2)
		newX += SPRITE_WIDTH / 2;
	else if (getY() > VIEW_HEIGHT / 2)
		newX -= SPRITE_WIDTH / 2;


	T* child = new T(newX, newY, getWorld());
	getWorld()->addActor(child);
}

Salmonella::Salmonella(int startX, int startY, StudentWorld* world)
	: Bacteria(IID_SALMONELLA, startX, startY, world) {
	m_movementPlanDistance = 0;
	setHealth(4);
}

void Salmonella::update() {
	if (TryDamage(1)) goto move;
	if (TryDivide()) goto move;
	TryEat();
move:
	if (TryMove()) return;
	if (GetFood()) return;
}

bool Bacteria::TryDamage(int dmg) {
	Socrates* socrates;
	if (getWorld()->getOverlapSocrates(this, socrates)) {
		socrates->updateHealth(-1 * dmg);
		return true;
	}
	else return false;
}

bool Bacteria::TryDivide() {
	if (m_foodSinceLast < 3) return false;

	Duplicate();

	m_foodSinceLast = 0;

	return true;
}

bool Bacteria::TryEat() {
	Actor* food;
	if (getWorld()->getOverlapObject(this, food, [](Actor* a) { return a->isFood(); })) {
		m_foodSinceLast++;
		food->updateHealth(-1);
		return true;
	}
	return false;
}

bool Salmonella::TryMove() {
	if (m_movementPlanDistance <= 0) return false;

	m_movementPlanDistance--;

	bool blocked = false;
	double x, y;
	getPositionInThisDirection(getDirection(), 3, x, y);

	Actor* blocker;
	if (getWorld()->getOverlapObject(x, y, blocker, [](Actor* a) { return a->doesBlock(); }, SPRITE_WIDTH / 2)) {
		blocked = true;
	}

	if (euclideanDistance(x, y, VIEW_WIDTH / 2, VIEW_HEIGHT / 2) >= VIEW_RADIUS) {
		blocked = true;
	}

	if (!blocked) {
		moveTo(x, y);
	}
	else {
		newMovementPlan();
	}

	return true;
}

void Salmonella::newMovementPlan() {
	int dir = randInt(0, 359);
	setDirection(dir);
	m_movementPlanDistance = 10;
}

bool Salmonella::GetFood() {
	Actor* food;
	if (!getWorld()->getClosestObject(this, food, [](Actor* a) { return a->isFood(); }, 128)) {
		// No food found
		newMovementPlan();
		return false;
	}

	// We got food!

	double x, y;

	double dirToFood = dirTowards(getX(), getY(), food->getX(), food->getY());

	getPositionInThisDirection(dirToFood, 3, x, y);

	Actor* blocker;
	if (getWorld()->getOverlapObject(x, y, blocker, [](Actor* a) { return a->doesBlock(); })) {
		newMovementPlan();
		return false;
	}

	setDirection(dirToFood);
	moveTo(x, y);
	return true;
}

void Bacteria::updateHealth(int health) {
	Actor::updateHealth(health);
	if (health < 0) {
		if (Alive())
			getWorld()->playSound(getSound("hurt"));
		else {
			getWorld()->playSound(getSound("die"));
			getWorld()->increaseScore(100);
			getWorld()->updateBugCount(-1);
			if (randInt(0, 1) == 0) { // 50% chance
				Actor* food = new Food(getX(), getY(), getWorld());
				getWorld()->addActor(food);
			}
		}
	}
}

AggSalmonella::AggSalmonella(int startX, int startY, StudentWorld* world)
	: Salmonella(startX, startY, world) {
	setHealth(10);
}

void AggSalmonella::update() {
	bool gotSoc = CheckForSocrates();
	if (TryDamage(2)) goto move;
	if (TryDivide()) goto move;
	TryEat();
move:
	if (gotSoc) return;
	if (TryMove()) return;
	if (GetFood()) return;
}

bool AggSalmonella::CheckForSocrates() {
	Socrates* soc;
	if (!getWorld()->getOverlapSocrates(this, soc, 72)) {
		return false;
	}

	double dirToSoc = dirTowards(getX(), getY(), soc->getX(), soc->getY());

	double x, y;
	getPositionInThisDirection(dirToSoc, 3, x, y);

	setDirection(dirToSoc);

	Actor* blocker;
	if (!getWorld()->getOverlapObject(x, y, blocker, [](Actor* a) { return a->doesBlock(); }, SPRITE_WIDTH / 2)) {
		moveTo(x, y);
	}

	return true;
}

EColi::EColi(int startX, int startY, StudentWorld* world)
	: Bacteria(IID_ECOLI, startX, startY, world) {
	setHealth(5);
}

void EColi::update() {
	if (TryDamage(4)) goto move;
	if (TryDivide()) goto move;
	TryEat();
move:
	CheckForSocrates();
}

bool EColi::CheckForSocrates() {
	Socrates* soc;
	if (!getWorld()->getOverlapSocrates(this, soc, 256)) {
		return false;
	}
	double dirToSoc = dirTowards(getX(), getY(), soc->getX(), soc->getY());

	for (int i = 0; i < 10; i++) {
		dirToSoc += 10;
		if (dirToSoc >= 360) dirToSoc -= 360;

		setDirection(dirToSoc);

		double x, y;
		getPositionInThisDirection(dirToSoc, 2, x, y);

		Actor* blocker;
		if (!getWorld()->getOverlapObject(x, y, blocker, [](Actor* a) { return a->doesBlock(); }, SPRITE_WIDTH / 2)) {
			moveTo(x, y);
			return true;
		}
	}

	return false;
}

// AMAZING GOODIES!! //

Pickup::Pickup(int imageId, int startX, int startY, int life, StudentWorld* world)
	: Actor(imageId, startX, startY, 0, 1, 1, world) {
	setHealth(1);
	m_lifetime = life;
	m_elapsedTicks = 0;
}

void Pickup::update() {
	Socrates* soc;
	if (getWorld()->getOverlapSocrates(this, soc)) {
		if (isGoodie())
			getWorld()->playSound(SOUND_GOT_GOODIE);
		setHealth(0);
		apply(soc);
		return;
	}
	m_elapsedTicks++;
	if (m_elapsedTicks >= m_lifetime) {
		setHealth(0);
	}
}

Goodie::Goodie(int imageId, int startX, int startY, int life, StudentWorld* world)
	: Pickup(imageId, startX, startY, life, world) {
}

HealthGoodie::HealthGoodie(int startX, int startY, int life, StudentWorld* world) 
	: Goodie(IID_RESTORE_HEALTH_GOODIE, startX, startY, life, world) {
}

void HealthGoodie::apply(Socrates* socrates) {
	socrates->updateHealthToMax();
	getWorld()->increaseScore(250);
}

FlameGoodie::FlameGoodie(int startX, int startY, int life, StudentWorld* world)
	: Goodie(IID_FLAME_THROWER_GOODIE, startX, startY, life, world) {
}

void FlameGoodie::apply(Socrates* socrates) {
	socrates->addFlames(5);
	getWorld()->increaseScore(300);
}

LifeGoodie::LifeGoodie(int startX, int startY, int life, StudentWorld* world)
	: Goodie(IID_EXTRA_LIFE_GOODIE, startX, startY, life, world) {
}

void LifeGoodie::apply(Socrates* socrates) {
	getWorld()->incLives();
	getWorld()->increaseScore(500);
}

Fungus::Fungus(int startX, int startY, int life, StudentWorld* world)
	: Pickup(IID_FUNGUS, startX, startY, life, world) {
}

void Fungus::apply(Socrates* socrates) {
	socrates->updateHealth(-20);
	getWorld()->increaseScore(-50);
}
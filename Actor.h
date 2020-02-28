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
	virtual void updateHealth(int health);

	virtual bool isDamageable() { return false; }
	virtual bool doesBlock() { return false; }
	virtual bool blockPlacement() { return false; }
	virtual bool isFood() { return false; }

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
	virtual bool doesBlock() { return true; }

	virtual void update();
};

class Food : public Actor {

public:
	Food(double startX, double startY, StudentWorld* world);

	virtual void update();

	virtual bool blockPlacement() { return true; }
	virtual bool isFood() { return true; }
};

class Socrates : public Actor {
	const int MAX_SPRAYS = 20;
	const int START_FLAMES = 5;
public:
	Socrates(StudentWorld* world);

	virtual bool isDamageable() { return true; }

	virtual void updateHealth(int health);
	void updateHealthToMax() {
		setHealth(100);
	}
	void addFlames(int amt) {
		m_flamesRemaining += amt;
	}

	virtual void update();

	int FlamesRemaining() {
		return m_flamesRemaining;
	}

	int SpraysRemaining() {
		return m_spraysRemaining;
	}

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

class Pit : public Actor {
public:
	Pit(double startX, double startY, StudentWorld* world);

	virtual bool blockPlacement() { return true; }

	virtual void update();

private:
	int m_regSalmonella;
	int m_aggSalmonella;
	int m_ecoli;
};

class Bacteria : public Actor {
public:
	Bacteria(int imageId, int startX, int startY, StudentWorld* world);

	virtual bool isDamageable() { return true; }
	virtual void updateHealth(int health);

protected:
	template<typename T>
	void duplicate();
	bool TryDamage(int damage);
	bool TryDivide();
	bool TryEat();
	virtual void Duplicate() = 0;

private:
	int m_foodSinceLast;
	virtual int getSound(std::string type) = 0;
};

class Salmonella : public Bacteria {
public:
	Salmonella(int startX, int startY, StudentWorld* world);

	virtual void update();

protected:
	bool TryMove();
	bool GetFood();
	void newMovementPlan();
	virtual void Duplicate() { duplicate<Salmonella>(); }

private:
	virtual int getSound(std::string type) {
		if (type == "hurt")
			return SOUND_SALMONELLA_HURT;
		else if (type == "die")
			return SOUND_SALMONELLA_DIE;
		return 0;
	}

	int m_movementPlanDistance;
};

class AggSalmonella : public Salmonella {
public:
	AggSalmonella(int startX, int startY, StudentWorld* world);

	virtual void update();

protected:
	bool CheckForSocrates();
	virtual void Duplicate() { duplicate<AggSalmonella>(); }
};

class EColi : public Bacteria {
public:
	EColi(int startX, int startY, StudentWorld* world);

	virtual void update();

protected:
	bool CheckForSocrates();
	virtual void Duplicate() { duplicate<EColi>(); }

private:
	virtual int getSound(std::string type) {
		if (type == "hurt")
			return SOUND_ECOLI_HURT;
		else if (type == "die")
			return SOUND_ECOLI_DIE;
		return 0;
	}
};

class Pickup : public Actor {
public:
	Pickup(int imageID, int startX, int startY, int life, StudentWorld* world);

	virtual void update();
	virtual void apply(Socrates* socrates) = 0;

	virtual bool isDamageable() { return true; }
	virtual bool isGoodie() { return false; }

protected:
	int m_lifetime;
	int m_elapsedTicks;
};

class Goodie : public Pickup {
public:
	Goodie(int imageID, int startX, int startY, int life, StudentWorld* world);

	virtual bool isGoodie() { return true; }
};

class HealthGoodie : public Goodie {
public:
	HealthGoodie(int startX, int startY, int life, StudentWorld* world);

private:
	virtual void apply(Socrates* socrates);
};

class FlameGoodie : public Goodie {
public:
	FlameGoodie(int startX, int startY, int life, StudentWorld* world);

private:
	virtual void apply(Socrates* socrates);
};

class LifeGoodie : public Goodie {
public:
	LifeGoodie(int startX, int startY, int life, StudentWorld* world);

private:
	virtual void apply(Socrates* socrates);
};

// I dont think a fungus is a goodie... but functionally they have a lot in common!
class Fungus : public Pickup {
public:
	Fungus(int startX, int startY, int life, StudentWorld* world);

private:
	virtual void apply(Socrates* socrates);
};

#endif // ACTOR_H_

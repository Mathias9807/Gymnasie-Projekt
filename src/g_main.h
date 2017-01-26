/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#ifndef G_MAIN_H
#define G_MAIN_H


#include "def.h"

struct Ship;
typedef struct Ship Ship;
struct Bullet;
typedef struct Bullet Bullet;

// Ett skepps logik
typedef struct Ai {
	void (*spawn)(struct Ship*);
	void (*live)(struct Ship*);
	void (*die)(struct Ship*);

	void (*onHit)(struct Ship*, Bullet*);
	void (*onDying)(struct Ship*);
} Ai;

#define G_SHIP_DYING_TIME 1.0
struct Ship {
	vec3 pos, vel;
	mat4x4 rot;
	float size; // Radius av skeppets hitbox

	vec3 pOffs; // Visuella förskjutningar, används för animationer
	float zOffs;

	// För accelerationsfunktionen
	float accT, accTFactor, accSpeed;

	float baseSpeed;

	int health; // Skeppets HP
	double ex0, ex1; // Timers för skeppets "exhaust"
	double deadTime; // Håller tiden då skeppet fick health == 0
	double flameTimer; // Används för att skjuta ut eld at death
	Ai* ai; // Skeppets interna logik
};

extern List G_ships;
extern Ship* G_player;

typedef struct {
	int texture;
	
	vec3 pos, vel;
	float scale, deltaScale;
	
	double spawnTime, lifeTime;
} Particle;
extern List G_particles;

struct Bullet {
	Ship* s;
	int texture;
	
	vec3 pos, vel;
	float scale, deltaScale;
	
	double spawnTime, lifeTime;
};
extern List G_bullets;

// Because windows is the best OS
#undef near
#undef far

typedef struct {
	// Absoluta koordinater
	vec3 pos;
	vec3 rot;

	// Alternativt relativa koordinater
	// Om focus == 0 används absoluta koordinater
	Ship* focus;
	Ship* ghost;	// En kopia av focus som laggar efter lite
			// för att göra kamerarörelserna mjukare
	vec3 pOffs;
	vec2 rOffs;

	double fov, near, far;
} Camera;
extern Camera cam;

void G_InitLevel();
void G_Tick();

// Create a new ship and add it to G_ships
// 	vec3 p		Position of the ship or NULL if zeroed
// 	vec3 v		Speed of the ship or NULL if zeroed
// 	mat4x4 r	Rotation of the ship or NULL if zeroed
// 	Ai* ai		The ai of the created ship or NULL
//
Ship* G_AddShip(vec3 p, vec3 v, mat4x4 r, Ai* ai);
void G_DeleteShip(Ship* s);
void G_RotateShip(Ship* s, float x, float y, float z);
void G_ShipExhaust(Ship* s, vec3 offs, double* lastExhaust);

// Create a new particle
// 	int tex		Texture to use in the particle texture array
// 	vec3 pos	Position of the particle or NULL if zeroed
// 	vec3 vel	Speed of the particle or NULL if zeroed
// 	float scale	Size of the particle
// 	float life	How long the particle should live for
//
Particle* G_AddParticle(int tex, vec3 pos, vec3 vel, float scale, double life);

// Create a new bullet
// 	Ship* s		The ship that fired the bullet
// 	int tex	
// 	vec3 pos	Position of the bullet or NULL if zeroed
// 	vec3 vel	Speed of the bullet or NULL if zeroed
// 	float scale	Size of the bullet
// 	float life	How long the bullet should live for
//
Bullet* G_AddBullet(Ship* s, int tex, vec3 pos, vec3 vel, float scale, double life);


#endif // G_MAIN_H

#pragma once
#include "Entity.h"


enum EnemyStates {
	ES_ATTACKER,
	ES_DEFENDER,
	ES_RECON,
};


void E_UfoInit(Entity* ent);
void E_UfoTick(Entity* ent);
void E_UfoDraw(Entity* ent);
void E_UfoHurt(Entity* ent, Entity* attacker);

void E_WalkerInit(Entity* ent);
void E_WalkerTick(Entity* ent);
void E_WalkerDraw(Entity* ent);


//void E_AlienUfoBulletInit(Entity* ent);
void E_AlienUfoBulletTick(Entity* ent);
void E_AlienUfoBulletDraw(Entity* ent);


void E_HunterInit(Entity* ent);
void E_HunterTick(Entity* ent);
void E_HunterDraw(Entity* ent);
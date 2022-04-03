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


void E_WalkerInit(Entity* ent);
void E_WalkerTick(Entity* ent);
void E_WalkerDraw(Entity* ent);
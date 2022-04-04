#pragma once
#include "Entity.h"

enum UnitStates {
	US_ATTACKER,
	US_DEFENDER,
	US_RECON,
};




void PlayerJetInit(Entity* ent);
void PlayerJetTick(Entity* ent);
void PlayerJetDraw(Entity* ent);


void JetBulletInit(Entity* ent);
void JetBulletTick(Entity* ent);
void JetBulletDraw(Entity* ent);

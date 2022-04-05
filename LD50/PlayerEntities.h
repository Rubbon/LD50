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
void PlayerJetHurt(Entity* ent, Entity* attacker);


void JetBulletInit(Entity* ent);
void JetBulletTick(Entity* ent);
void JetBulletDraw(Entity* ent);


void MilJetInit(Entity* ent);
void MilJetTick(Entity* ent);
void MilJetDraw(Entity* ent);


void PlaneHurt(Entity* ent, Entity* attacker);

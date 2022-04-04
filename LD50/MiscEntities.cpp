#include "MiscEntities.h"
#include "Graphics.h"
#include "Game.h"




void FxInit(Entity* ent) {
	ent->mz = -3;
}

void FxTick(Entity* ent) {
	
	//die after timer
	ent->hp--;
	if (ent->hp <= 0) DeleteEntity(ent);



	if (ent->z >= 0) {
		if (ent->flags & FXS_DESTROY_ON_LAND) DeleteEntity(ent);
	} else {
		if (ent->flags & FXS_HAS_GRAVITY) {
			if (ent->mz < 3) ent->mz += 0.25f;
		}
	}


	//move
	ent->fx += ent->mx;
	ent->fy += ent->my;
	ent->z += ent->mz;

	ent->x = ent->fx;
	ent->y = ent->fy;

}

void FxDraw(Entity* ent) {
	Graphics::DrawSpr(TEX_CHARS, { ent->x - CAMERA_X - ent->animSpr.w / 2, ent->y - CAMERA_Y, ent->animSpr.w, ent->animSpr.h }, ent->animSpr, ent->blend);
}

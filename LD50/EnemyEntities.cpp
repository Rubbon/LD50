#include "EnemyEntities.h"
#include "Graphics.h"
#include "Game.h"




void E_UfoInit(Entity* ent) {

}


void E_UfoTick(Entity* ent) {

	ent->z = -6 + sin((GAME_TICK + ent->id)*0.05f) * 2;

	//move to target pos
	if (ent->x < ent->target_x) ent->x++;
	else if (ent->x > ent->target_x) ent->x--;

	if (ent->y < ent->target_y) ent->y++;
	else if (ent->y > ent->target_y) ent->y--;

	switch (ent->state) {
		case ES_RECON:
			if (ent->substate != 2 && ent->x == ent->target_x && ent->y == ent->target_y) {


				//report what we see
				if (ent->wait == 0 && ent->substate == 0) {
					Tile* _tile = LEVEL.GetTile(ent->x >> 3, ent->y >> 3);

					switch (_tile->type) {
						case TT_CITYBLOCK_BIG: case TT_CITYBLOCK_SMALL: case TT_CITY_BANK:
							std::cout << "FOUND CITY " << std::endl;
							LEVEL.arrCities[_tile->owner].flags |= CF_FOUND;
							ent->substate = 1;
						break;
						case TT_HQ_TL: case TT_HQ_TR: case TT_HQ_BL: case TT_HQ_BR:
							std::cout << "FOUND HQ " << std::endl;
							GAME.alienMastermind.foundHqPos = {(short)(ent->x >> 3), (short)(ent->y >> 3)};
							ent->substate = 1;
						break;
					}

				}


				ent->wait++;

				if (ent->wait >= 80) {
					ent->wait = 0;
					ent->ticker++;

					int _xDir = 1;
					if (ent->ticker / RECON_SPACE_SIZE % 2 != 0) _xDir = -1;
				
					//move
					if (ent->ticker % RECON_SPACE_SIZE == 0) ent->target_y += 8;
					else  ent->target_x += 8 * _xDir;
					
					//we're done searching
					if (ent->ticker >= RECON_SPACE_SIZE * RECON_SPACE_SIZE) {
						ent->substate = 2;
					}

				}

			}
		break;

		case ES_ATTACKER:

			//if we're at destination, consider moving around a little

		break;

	}
}


void E_UfoDraw(Entity* ent) {
	//shadow
	Graphics::DrawSpr(TEX_CHARS, {ent->x - 6 - ent->z - CAMERA_X, ent->y + 1 - CAMERA_Y, 8, 3 }, {0, 157, 8, 3});
	//me
	Graphics::DrawSpr(TEX_CHARS, {ent->x - 4 -CAMERA_X, ent->y - 4 + ent->z - CAMERA_Y, 8, 5 }, {0, 152, 8, 5});
}








const SDL_Rect arrAnimWalkerWalk[4] = { {0, 179, 9, 10}, {9, 179, 7, 10}, {0, 179, 9, 10},  {17, 179, 7, 10} };


void E_WalkerInit(Entity* ent) {

}


void E_WalkerTick(Entity* ent) {
	if (GAME_TICK % 16 == 0) {
		ent->animFrame++;
		if (ent->animFrame > 3) ent->animFrame = 0;
	}
}


void E_WalkerDraw(Entity* ent) {
	//shadow
	Graphics::DrawSpr(TEX_CHARS, { ent->x - 4 - CAMERA_X, ent->y - 1 - CAMERA_Y, 8, 2 }, { 24, 187, 8, 2 });
	//me
	Graphics::DrawSpr(TEX_CHARS, { ent->x - 1 - arrAnimWalkerWalk[ent->animFrame].w/2 - CAMERA_X, ent->y - 10 + ent->z - CAMERA_Y, arrAnimWalkerWalk[ent->animFrame].w, arrAnimWalkerWalk[ent->animFrame].h}, arrAnimWalkerWalk[ent->animFrame]);
}



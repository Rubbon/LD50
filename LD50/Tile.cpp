#include "Tile.h"
#include "Graphics.h"


void TileDraw(int dx, int dy, int tx, int ty, Tile* _tile) {
	switch (_tile->type) {
		case TT_LAND:
			DrawLand(dx, dy, tx, ty);
		break;
	}
}


void DrawLand(int dx, int dy, int tx, int ty) {
	Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 0, 0, 8, 8 });
}

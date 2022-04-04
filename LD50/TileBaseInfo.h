#include "Tile.h"


const TileInfo arrTileInfo[] = {
	{
		/* NAME */			"Air",
		/* BASE_HP */		0,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
		/* FLAGS */			0x00,
	},
	{
		/* NAME */			"Water",
		/* BASE_HP */		0,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
		/* FLAGS */			0x00,
	},
	{
		/* NAME */			"Land",
		/* BASE_HP */		0,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
		/* FLAGS */			TIF_WALKABLE,
	},
	{
		/* NAME */			"Tree",
		/* BASE_HP */		2,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
		/* FLAGS */			0x00,
	},
	{
		/* NAME */			"City", // small
		/* BASE_HP */		6,
		/* BUILD_SPR */		{8, 64, 16, 16},
		/* BUILD_TIME */	1280,
		/* FLAGS */			TIF_HUMAN,
	},
	{
		/* NAME */			"City", // big
		/* BASE_HP */		6,
		/* BUILD_SPR */		{8, 64, 16, 16},
		/* BUILD_TIME */	3000,
		/* FLAGS */			TIF_HUMAN,
	},
	{
		/* NAME */			"HQ", //tl
		/* BASE_HP */		16,
		/* BUILD_SPR */		{8, 64, 16, 16},
		/* BUILD_TIME */	128,
		/* FLAGS */			TIF_HUMAN,
	},
	{
		/* NAME */			"HQ", //tr
		/* BASE_HP */		16,
		/* BUILD_SPR */		{8, 64, 16, 16},
		/* BUILD_TIME */	128,
		/* FLAGS */			TIF_HUMAN,
	},
	{
		/* NAME */			"HQ", //bl
		/* BASE_HP */		16,
		/* BUILD_SPR */		{8, 64, 16, 16},
		/* BUILD_TIME */	128,
		/* FLAGS */			TIF_HUMAN,
	},
	{
		/* NAME */			"HQ", //br
		/* BASE_HP */		16,
		/* BUILD_SPR */		{8, 64, 16, 16},
		/* BUILD_TIME */	128,
		/* FLAGS */			TIF_HUMAN,
	},
	{
		/* NAME */			"Bank",
		/* BASE_HP */		10,
		/* BUILD_SPR */		{8, 64, 16, 16},
		/* BUILD_TIME */	5000,
		/* FLAGS */			TIF_HUMAN,
	},
	{
		/* NAME */			"Construction Site",
		/* BASE_HP */		4,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
		/* FLAGS */			TIF_TICKABLE | TIF_HUMAN,
	},
	{
		/* NAME */			"Crater",
		/* BASE_HP */		0,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
		/* FLAGS */			TIF_WALKABLE,
	},
	{
		/* NAME */			"Factory", //tl
		/* BASE_HP */		0,
		/* BUILD_SPR */		{48, 64, 16, 16},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
	},
	{
		/* NAME */			"Factory", //tr
		/* BASE_HP */		0,
		/* BUILD_SPR */		{48, 64, 16, 16},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
	},
	{
		/* NAME */			"Factory", //bl
		/* BASE_HP */		0,
		/* BUILD_SPR */		{48, 64, 16, 16},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
	},
	{
		/* NAME */			"Factory", //br
		/* BASE_HP */		0,
		/* BUILD_SPR */		{48, 64, 16, 16},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
	},
	{
		/* NAME */			"Airfield", //tl
		/* BASE_HP */		0,
		/* BUILD_SPR */		{0, 128, 16, 16},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
	},
	{
		/* NAME */			"Airfield", //tr
		/* BASE_HP */		0,
		/* BUILD_SPR */		{0, 128, 16, 16},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
	},
	{
		/* NAME */			"Airfield", //bl
		/* BASE_HP */		0,
		/* BUILD_SPR */		{0, 128, 16, 16},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
	},
	{
		/* NAME */			"Airfield", //br
		/* BASE_HP */		0,
		/* BUILD_SPR */		{0, 128, 16, 16},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
	},
	{
		/* NAME */			"Anti Air Gun",
		/* BASE_HP */		4,
		/* BUILD_SPR */		{0, 96, 8, 8},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN | TIF_TICKABLE,
	},
	{
		/* NAME */			"Wall",
		/* BASE_HP */		4,
		/* BUILD_SPR */		{64, 96, 8, 8},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
	},

};












//for accessing the tileInfo array without having to use its ugly name and []
#define GET_TILE_INFO(_tt) (arrTileInfo[_tt])
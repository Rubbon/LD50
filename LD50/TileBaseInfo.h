#include "Tile.h"


const TileInfo arrTileInfo[] = {
	{
		/* NAME */			"Demolish",
		/* BASE_HP */		0,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
		/* FLAGS */			0x00,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"Water",
		/* BASE_HP */		0,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
		/* FLAGS */			0x00,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"Land",
		/* BASE_HP */		0,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
		/* FLAGS */			TIF_WALKABLE,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"Tree",
		/* BASE_HP */		2,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
		/* FLAGS */			0x00,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"City", // small
		/* BASE_HP */		6,
		/* BUILD_SPR */		{8, 64, 16, 16},
		/* BUILD_TIME */	1280,
		/* FLAGS */			TIF_HUMAN,
		/* BUILDCOST */		0,
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
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"HQ", //tr
		/* BASE_HP */		16,
		/* BUILD_SPR */		{8, 64, 16, 16},
		/* BUILD_TIME */	128,
		/* FLAGS */			TIF_HUMAN,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"HQ", //bl
		/* BASE_HP */		16,
		/* BUILD_SPR */		{8, 64, 16, 16},
		/* BUILD_TIME */	128,
		/* FLAGS */			TIF_HUMAN,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"HQ", //br
		/* BASE_HP */		16,
		/* BUILD_SPR */		{8, 64, 16, 16},
		/* BUILD_TIME */	128,
		/* FLAGS */			TIF_HUMAN,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"Bank",
		/* BASE_HP */		10,
		/* BUILD_SPR */		{8, 64, 16, 16},
		/* BUILD_TIME */	5000,
		/* FLAGS */			TIF_HUMAN,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"Construction Site",
		/* BASE_HP */		4,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
		/* FLAGS */			TIF_TICKABLE | TIF_HUMAN,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"Crater",
		/* BASE_HP */		0,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
		/* FLAGS */			TIF_WALKABLE,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"Factory", //tl
		/* BASE_HP */		0,
		/* BUILD_SPR */		{48, 64, 16, 16},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"Factory", //tr
		/* BASE_HP */		0,
		/* BUILD_SPR */		{48, 64, 16, 16},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"Factory", //bl
		/* BASE_HP */		0,
		/* BUILD_SPR */		{48, 64, 16, 16},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"Factory", //br
		/* BASE_HP */		0,
		/* BUILD_SPR */		{48, 64, 16, 16},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"Airfield", //tl
		/* BASE_HP */		0,
		/* BUILD_SPR */		{0, 128, 16, 16},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"Airfield", //tr
		/* BASE_HP */		0,
		/* BUILD_SPR */		{0, 128, 16, 16},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"Airfield", //bl
		/* BASE_HP */		0,
		/* BUILD_SPR */		{0, 128, 16, 16},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"Airfield", //br
		/* BASE_HP */		0,
		/* BUILD_SPR */		{0, 128, 16, 16},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"AA Gun",
		/* BASE_HP */		4,
		/* BUILD_SPR */		{0, 96, 8, 8},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN | TIF_TICKABLE,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"Wall",
		/* BASE_HP */		4,
		/* BUILD_SPR */		{64, 96, 8, 8},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"City Plans",
		/* BASE_HP */		6,
		/* BUILD_SPR */		{56, 56, 8, 8},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"Rail Station", // H
		/* BASE_HP */		6,
		/* BUILD_SPR */		{88, 88, 8, 8},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN | TIF_RAIL,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"Rail Station", // V
		/* BASE_HP */		6,
		/* BUILD_SPR */		{88, 88, 8, 8},
		/* BUILD_TIME */	256,
		/* FLAGS */			TIF_HUMAN | TIF_RAIL,
		/* BUILDCOST */		0,
	},
	{
		/* NAME */			"Rail", // H
		/* BASE_HP */		3,
		/* BUILD_SPR */		{104, 88, 8, 8},
		/* BUILD_TIME */	64,
		/* FLAGS */			TIF_HUMAN | TIF_RAIL,
		/* BUILDCOST */		0,
	},


};












//for accessing the tileInfo array without having to use its ugly name and []
#define GET_TILE_INFO(_tt) (arrTileInfo[_tt])
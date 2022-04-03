#include "Tile.h"


const TileInfo arrTileInfo[] = {
	{
		/* NAME */			"Air",
		/* BASE_HP */		0,
		/* BUILD_SPR */		{},
	},
	{
		/* NAME */			"Water",
		/* BASE_HP */		0,
		/* BUILD_SPR */		{},
	},
	{
		/* NAME */			"Land",
		/* BASE_HP */		0,
		/* BUILD_SPR */		{},
	},
	{
		/* NAME */			"Tree",
		/* BASE_HP */		2,
		/* BUILD_SPR */		{},
	},
	{
		/* NAME */			"City", // small
		/* BASE_HP */		6,
		/* BUILD_SPR */		{},
	},
	{
		/* NAME */			"City", // big
		/* BASE_HP */		6,
		/* BUILD_SPR */		{},
	},
	{
		/* NAME */			"HQ", //tl
		/* BASE_HP */		16,
		/* BUILD_SPR */		{8, 64, 16, 16},
	},
	{
		/* NAME */			"HQ", //tr
		/* BASE_HP */		16,
		/* BUILD_SPR */		{8, 64, 16, 16},
	},
	{
		/* NAME */			"HQ", //bl
		/* BASE_HP */		16,
		/* BUILD_SPR */		{8, 64, 16, 16},
	},
	{
		/* NAME */			"HQ", //br
		/* BASE_HP */		16,
		/* BUILD_SPR */		{8, 64, 16, 16},
	},
	{
		/* NAME */			"Bank",
		/* BASE_HP */		10,
		/* BUILD_SPR */		{},
	},



};












//for accessing the tileInfo array without having to use its ugly name and []
#define GET_TILE_INFO(_tt) (arrTileInfo[_tt])
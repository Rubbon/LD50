#include "Tile.h"


const TileInfo arrTileInfo[] = {
	{
		/* NAME */			"Air",
		/* BASE_HP */		0,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
	},
	{
		/* NAME */			"Water",
		/* BASE_HP */		0,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
	},
	{
		/* NAME */			"Land",
		/* BASE_HP */		0,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
	},
	{
		/* NAME */			"Tree",
		/* BASE_HP */		2,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
	},
	{
		/* NAME */			"City", // small
		/* BASE_HP */		6,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
	},
	{
		/* NAME */			"City", // big
		/* BASE_HP */		6,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
	},
	{
		/* NAME */			"HQ", //tl
		/* BASE_HP */		16,
		/* BUILD_SPR */		{8, 64, 16, 16},
		/* BUILD_TIME */	8,
	},
	{
		/* NAME */			"HQ", //tr
		/* BASE_HP */		16,
		/* BUILD_SPR */		{8, 64, 16, 16},
		/* BUILD_TIME */	8,
	},
	{
		/* NAME */			"HQ", //bl
		/* BASE_HP */		16,
		/* BUILD_SPR */		{8, 64, 16, 16},
		/* BUILD_TIME */	8,
	},
	{
		/* NAME */			"HQ", //br
		/* BASE_HP */		16,
		/* BUILD_SPR */		{8, 64, 16, 16},
		/* BUILD_TIME */	8,
	},
	{
		/* NAME */			"Bank",
		/* BASE_HP */		10,
		/* BUILD_SPR */		{},
		/* BUILD_TIME */	0,
	},



};












//for accessing the tileInfo array without having to use its ugly name and []
#define GET_TILE_INFO(_tt) (arrTileInfo[_tt])
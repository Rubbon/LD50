#pragma once


#define DIR_NORTH 0
#define DIR_EAST 1
#define DIR_SOUTH 2
#define DIR_WEST 3


enum EntityFlags {
	EFL_DELETED = 0x01,
	// = 0x02,
	// = 0x04,
	// = 0x08,
	// = 0x10,
	// = 0x20,
	// = 0x40,
	// = 0x80,
};


struct Entity {
	int x;
	int y;
	int draw_x;
	int draw_y;
	unsigned char dir = DIR_NORTH;
	unsigned char flags = EFL_DELETED;
	unsigned char entityIndex;
	unsigned short id;
};



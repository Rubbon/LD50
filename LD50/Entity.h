#pragma once


enum EntityName {
	ENT_UFO,
	ENT_WALKER,

};


enum EntityFlags {
	EFL_DELETED = 0x01,
	EFL_AIR = 0x02,
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
	short z;
	unsigned char flags = EFL_DELETED;
	unsigned char entityIndex;
	unsigned short id;

	short currentChunk = -1;


	short target_x;
	short target_y;

	short hp = 0;

	//animation
	unsigned char animFrame = 0;


	char state;
	short substate;

	//generic values
	short wait;
	short ticker;


};



//weird syntax because of std::sort, used to sort entities by their Y position so they draw on top of one-another as you'd expect
struct EntitySorter {
	inline bool operator() (const Entity* ent1, const Entity* ent2) {
		return (ent1->y + ent1->z < ent2->y + ent1->z);
	}
};


//for function pointers to access our entity functions.
typedef void (*EntityFunc)(Entity*);
typedef void (*EntityEntityFunc)(Entity*, Entity*);


//use this if you want to specify an entity function as none
extern void NoFunc(Entity* _entity);
extern void NoFunc(Entity* _entity_1, Entity* _entity_2);

//all the different function types that your entity can use are listed here
//probably good to discuss before adding to this
struct EntityFunctions {
	EntityFunc Init = NoFunc; // run on spawn
	EntityFunc Tick = NoFunc; // run every frame
	EntityFunc Draw = NoFunc; // run at end of every frame
	EntityEntityFunc OnHurt = NoFunc;	// run when the entity has been hurt by another
	EntityEntityFunc OnHurtOther = NoFunc; // run when the entity itself has hurt another
	EntityEntityFunc OnDestroy = NoFunc; // run when the entity is destroyed
};

extern const EntityFunctions arrEntityFuncs[];



//long winded function for making sure entities are in the right chunks
extern void SortEntityIntoCorrectChunk(Entity* _ent);

extern short GetChunkIndexAtEntityPos(int x, int y);
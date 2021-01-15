#pragma once
#include "includes.h"
// File for all the byte pattern and reClass structs

#define CHECK_BAD_PTR(x) if(IsBadReadPtr(this,sizeof(x))) return nullptr
#define CHECK_BAD(x) if(IsBadReadPtr(this, sizeof(x))) return
#define CHECK_BAD_NUM(x) if(IsBadReadPtr(this, sizeof(x))) return 0
#define CHECK_BAD_VEC3(x) if(IsBadReadPtr(this, sizeof(x))) return Vector3{0,0,0}

//Example stuff following

struct Vector3 { float x, y, z; };

class Entity
{
public:
	char pad_0000[16]; //0x0000
	char name[24]; //0x0010
	char pad_0028[692]; //0x0028
	Vector3 position; //0x02DC
	int32_t isPlayerCharacter; //0x02E8 // if player 3F800000
	char pad_02EC[84]; //0x02EC
	int32_t isRendered; //0x0340
	char pad_0344[924]; //0x0344
	int32_t attackStance; //0x06E0
	char pad_06E4[4]; //0x06E4
	int32_t mobID; //0x06E8
	char pad_06EC[68]; //0x06EC
	int32_t mobDiedRecently; //0x0730 1 if died recently, 0 otherwise
	char pad_0734[5408]; //0x0734
	int32_t uID; //0x1C54
	char pad_1C58[4]; //0x1C58
	int8_t skipCollision; //0x1C5C
	char pad_1C5D[1019]; //0x1C5D

	void disableSkipCollision() { CHECK_BAD(Entity); skipCollision = 0x0; };
	void enableSkipCollision() { CHECK_BAD(Entity); skipCollision = 0x1; };
	int32_t getAttackStance() { CHECK_BAD_NUM(Entity); return attackStance; };
	Vector3 getPosition() { CHECK_BAD_VEC3(Entity); return position; };
	int8_t getSkipCollision() { CHECK_BAD_NUM(Entity); return skipCollision; };
	int32_t getIsPlayerCharacter() { CHECK_BAD_NUM(Entity); return isPlayerCharacter; };
	int32_t getIsRendered() { CHECK_BAD_NUM(Entity); return isRendered; };
	void setIsRendered(int32_t value) { CHECK_BAD(Entity); isRendered = value; };
	int32_t getMobDiedRecently() { CHECK_BAD_NUM(Entity); return mobDiedRecently; };
	int32_t getUid() { CHECK_BAD_NUM(Entity); return uID; };
	int32_t getMobId() { CHECK_BAD_NUM(Entity); return mobID; };

}; //Size: 0x2058
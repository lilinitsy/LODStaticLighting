#pragma once


// The ExperimentalParams struct will store all the condition data for
// one trial, where one trial is scene -- black -- scene -- black, with
// LOD popping happening in one of the "scenes", and not in the other.

enum ModelID
{
	CLAYPOT,
	WOOD_LOGS,
	CASTLE_STRUCTURE,
	CLAYJUG,
	ICELANDIC_SPIRE,
	VOLCANIC_ROCK,
	PINE_CONE,
	WOOD_STUMP,

	MODELID_COUNT,
};

enum Method
{
	UNIQUE, // comparator
	REUSE,  // ours

	METHOD_COUNT,
};


// Whether the pop will happen on the left side or the right side
enum LeftRight
{
	LEFT,
	RIGHT,
	CENTER,

	LEFTRIGHT_COUNT,
};


const uint32_t NUM_ECCENTRICITIES = 4;
const uint32_t NUM_DISTANCES      = 4;


struct ModelParams
{
	ModelID id;
	float   scale;
};


struct ExperimentalParams
{
	ModelParams model_id;
	uint8_t     lodlvl;
	Method      method;
	LeftRight   leftright;
	float       eccentricity;
	float       distance = 2.0f;
	float       model_movement_speed = 50.0f;
};
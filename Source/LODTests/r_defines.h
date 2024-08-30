#pragma once


// Unreal seems finnicky with comparisons with #define's? Will have to use const int values instead if not using them in preprocess commands
const uint8_t NUM_LODS = 4;

const bool TAKE_SCREENSHOTS = true;

const bool MIPMAP_SHARED_LIGHTMAP = true;

const float SCREEN_TIME       = 2.0f;
const float BLACK_TIME        = 0.5f;
const float DESIRED_FRAMERATE = 60.0f;
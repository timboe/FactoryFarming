#pragma once
#include "game.h"

// Chunk quadrants
enum kChunkQuad{NE, SE, SW, NW};

struct Player_t{
  // Persistent
  float m_pix_x; // Note: Centre
  float m_pix_y;

  // Transient   
  LCDSprite* m_sprite[ZOOM_LEVELS]; // Index 0 not used - OWNED
  LCDSprite* m_blueprint[ZOOM_LEVELS]; // Index 0 not used - OWNED
  float m_vX;
  float m_vY;
};

struct Player_t* getPlayer(void);

int16_t getOffX(void);

int16_t getOffY(void);

struct Chunk_t* getCurrentChunk(void);

enum kChunkQuad getCurrentQuadrant(void);

struct Location_t* getPlayerLocation(void);

struct Location_t* getPlayerLookingAtLocation(void);

bool movePlayer(void);

void initPlayer(void);

void resetPlayer(void);

void serialisePlayer(struct json_encoder* je); 

void didDecodeTableValuePlayer(json_decoder* jd, const char* _key, json_value _value);

void* deserialiseStructDonePlayer(json_decoder* jd, const char* _name, json_value_type _type);
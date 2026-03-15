#pragma once

#include "raylib.h"

enum class EnemyType
{
	GRUNT,
	SCOUT,
	ARMORED,
	SPLITTER,
	SPLITTER_SMALL
};

class EnemyFactory
{
public:
	struct EnemyConfig
	{
		float speed;
		int health;
		float radius;
		Color color;
		
	};

	static EnemyConfig GetConfig(EnemyType type)
	{
		switch (type)
		{
		case EnemyType::GRUNT:
			return { 120.0f, 30, 20.0f, RED };
		case EnemyType::SCOUT:
			return { 180.0f , 15, 14.0f, ORANGE };
		case EnemyType::ARMORED:
			return { 80.0f, 100, 59.4f, DARKGRAY };
		case EnemyType::SPLITTER:
			return { 50.0f, 100, 40.0f, PURPLE };
		case EnemyType::SPLITTER_SMALL:
			return { 110.0f, 40, 14.0f, VIOLET };
		default:
			return { 120.0f, 30, 20.0f, GRAY };
		}
	}

	static const char* GetEnemyName(EnemyType type)
	{
		switch (type)
		{
		case EnemyType::GRUNT: return "Grunt";
		case EnemyType::SCOUT: return "Scout";
		case EnemyType::ARMORED: return "Armored";
		case EnemyType::SPLITTER: return "Splitter";
		case EnemyType::SPLITTER_SMALL: return "Splitter (small)";
		default: return "Unknown";
		}
	}
};
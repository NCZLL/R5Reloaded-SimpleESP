#pragma once
#include "Memory\Memory.h"

namespace offset
{
	// Main
	constexpr auto dwEntityList		= 0x1F33F78;
	constexpr auto dwLocalPlayer	= 0x22E93F8;
	constexpr auto OFFSET_Matrix	= 0x1A93D0;
	constexpr auto OFFSET_Rnder		= 0xD4138F0;

	// Player
	constexpr auto m_iTeamNum		 = 0x3F0;		// int
	constexpr auto m_iHealth		 = 0x3E0;		// int
	constexpr auto m_shieldHealth	 = 0x170;		// int
	constexpr auto m_lifeState		 = 0x730;		// int
	constexpr auto m_localOrigin	 = 0x14C;		// Vector3
	constexpr auto m_iMaxHealth		 = 0x510;
	constexpr auto m_shieldHealthMax = 0x174;
	constexpr auto s_BoneMatrix		 = 0xEE0;
	constexpr auto m_localAngle = 0x428;
}
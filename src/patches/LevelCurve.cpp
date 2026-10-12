#include "pch.h"

// EDIT REQUIRED EXP PER LEVEL AND HERO MAX LEVEL
// The experience function (ECX = level, returns EAX = total experience needed to reach it) is replaced by
// hero_level_exp below; the hard-coded max level is patched in the 4 places that clamp levels.
// Replaces the static h5m_level-curve.yml edits already present in H5X.exe (same values).

void HeroLevelExpFork();

static const int hero_level_max = 50;  // <= 127 (used as a signed byte operand)

// Total experience needed to reach each level, index = level (level 1 = 0 exp).
static const int hero_level_exp[] = {
	0,          0,          1000,       2000,       3200,       4600,       6200,       8000,       10000,      12200,
	14700,      17500,      20600,      24300,      28700,      34100,      40500,      48200,      57400,      68500,
	81700,      97600,      116000,     139000,     167000,     200000,     239000,     286000,     343000,     411000,
	492000,     590000,     706000,     846000,     1010000,    1210000,    1450000,    1740000,    2080000,    2500000,
	3000000,    3600000,    4330000,    5230000,    6310000,    7640000,    9240000,    11200000,   13500000,   16400000,
	20000000,
};

static_assert(hero_level_max <= 127, "max level is also a signed byte operand");
static_assert(sizeof(hero_level_exp) / sizeof(hero_level_exp[0]) == hero_level_max + 1,
	"hero_level_exp needs one value per level from 0 to hero_level_max");

int HeroLevelExp_fork = 0x00B4C6B0;  // experience for level (25 callers)

void LevelCurve_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, HeroLevelExp_fork, 5, HeroLevelExpFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT,  0x00997AC0, 4, nullptr, hero_level_max, 0, 0, 0 });  // default map max level
	assembly_patches.push_back({ PATCH_BYTE, 0x009AA973, 1, nullptr, hero_level_max, 0, 0, 0 });  // cmp eax, max
	assembly_patches.push_back({ PATCH_INT,  0x009AA977, 4, nullptr, hero_level_max, 0, 0, 0 });  // mov eax, max
	assembly_patches.push_back({ PATCH_BYTE, 0x00B4C856, 1, nullptr, hero_level_max, 0, 0, 0 });  // exp -> level: cmp ecx, max
	assembly_patches.push_back({ PATCH_INT,  0x00B4C85A, 4, nullptr, hero_level_max, 0, 0, 0 });  // exp -> level: mov eax, max
	assembly_patches.push_back({ PATCH_BYTE, 0x0098C686, 1, nullptr, hero_level_max, 0, 0, 0 });  // map max level: cmp eax, max
	assembly_patches.push_back({ PATCH_INT,  0x0098C68A, 4, nullptr, hero_level_max, 0, 0, 0 });  // map max level: mov eax, max
}

static int __fastcall hero_level_exp_for(int level) {
	if (level > hero_level_max) return 0x7FFFFFFF;  // unreachable level
	if (level <= 1) return 0;
	return hero_level_exp[level];
}

// Callers rely on ECX (and EDX) being preserved, e.g. the exp -> level loop at 0x00B4C840.
__declspec(naked) void HeroLevelExpFork() {
	__asm
	{
		push ecx
		push edx
		call hero_level_exp_for
		pop edx
		pop ecx
		ret
	}
}

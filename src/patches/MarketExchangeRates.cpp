#include "pch.h"

// NEW MARKET EXCHANGE RATES
// FUN_008E7B80 picks the rates from three tables by sold resource (common = wood/ore, rare =
// mercury/crystal/sulfur/gems, gold), one row of 7 ints per market count (1..9, 9 = also neutral markets),
// one column per bought resource. Value = units sold per unit bought, or gold received per unit sold
// when buying gold.
// New row 1 (one market) = old row 9; same-category trades reach 1:1 at 9 markets / neutral markets.
// Week of Trade (week 0x34): rates of 3 more markets (capped at 9) instead of the vanilla halving of every
// value, which also halved the gold received when selling and allowed buy/convert/sell gold loops.

void MarketCountFork();

static const int market_tables[3] = { 0x00FF762C, 0x00FF772C, 0x00FF782C };  // + row * 0x1C, rows 1..9

//                                   common  rare  gold
static const int market_rates[3][9][3] = {
	{ // sell common
		{ 2, 4, 120 }, { 2, 4, 130 }, { 2, 4, 140 }, { 2, 4, 150 }, { 2, 3, 160 },
		{ 2, 3, 170 }, { 2, 3, 180 }, { 2, 3, 190 }, { 1, 2, 200 },
	},
	{ // sell rare
		{ 1, 2, 250 }, { 1, 2, 275 }, { 1, 2, 300 }, { 1, 2, 325 }, { 1, 2, 350 },
		{ 1, 2, 375 }, { 1, 2, 400 }, { 1, 2, 425 }, { 1, 1, 450 },
	},
	{ // sell gold (gold paid per unit)
		{ 500, 1000, 0 }, { 475, 950, 0 }, { 450, 900, 0 }, { 425, 850, 0 }, { 400, 800, 0 },
		{ 375, 750, 0 }, { 350, 700, 0 }, { 325, 650, 0 }, { 300, 600, 0 },
	},
};

int MarketCount_fork = 0x008E7BA7;    // cmp esi, 9; jle; mov esi, 9 (ESI = market count, already > 0)
int MarketCount_return = 0x008E7BB1;

void MarketExchangeRates_init(pugi::xml_document& doc) {
	static const int category[7] = { 0, 0, 1, 1, 1, 1, 2 };  // wood, ore, mercury, crystal, sulfur, gems, gold
	for (int table = 0; table < 3; table++) {
		for (int row = 1; row <= 9; row++) {
			// one PATCH_WRTE per row (7 little-endian ints)
			assembly_patch patch = { PATCH_WRTE, market_tables[table] + row * 0x1C, 0x1C, nullptr, 0, 0, 0, "" };
			for (int res = 0; res < 7; res++) {
				unsigned int value = market_rates[table][row - 1][category[res]];
				for (int b = 0; b < 4; b++)
					sprintf_s(patch.value_bytes + (res * 4 + b) * 2, 3, "%02X", (value >> (8 * b)) & 0xFF);
			}
			assembly_patches.push_back(patch);
		}
	}
	assembly_patches.push_back({ PATCH_HOOK, MarketCount_fork, 10, MarketCountFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_NOP, 0x008E7C0C, 5, nullptr, 0, 0, 0, 0 }); // call func_CheckWeekOfTradeRate
}

__declspec(naked) void MarketCountFork() {
	__asm
	{
		cmp dword ptr [esp + 0x30], 0x34    // week id (2nd stack arg)
		jne MARKET_COUNT_CLAMP
		add esi, 0x3

		MARKET_COUNT_CLAMP:
		cmp esi, 0x9
		jle MARKET_COUNT_RETURN
		mov esi, 0x9

		MARKET_COUNT_RETURN:
		jmp [MarketCount_return]
	}
}

#include "pch.h"
#include "structs/CombatUnit.h"
#include "structs/Creature.h"
#include "structs/Spell.h"

// PERK_UNSETTLE: earth damage spells apply a max-HP reduction of 100/max_hp per damaged stack

void UnsettleTargetedFork();
void UnsettleAreaFork();

int UnsettleTargeted_fork = 0x0097DF67;
int UnsettleTargeted_return = 0x0097DF6C;
int UnsettleArea_fork = 0x0085ECE2;
int UnsettleArea_return = 0x0085ECE7;

static bool __cdecl ShouldApplyUnsettle(int spell_id, ICombatUnit* caster);
static void __cdecl DoApplyUnsettle(ICombatUnit* target, int timeline);

void Unsettle_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, UnsettleTargeted_fork, 5, UnsettleTargetedFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, UnsettleArea_fork, 5, UnsettleAreaFork, 0, 0, 0 });
}

static bool __cdecl ShouldApplyUnsettle(int spell_id, ICombatUnit* caster) {
	if (!spell_id || !caster)
		return false;

	if (caster->unit_kind() != 2)
		return false;

	ICombatHero* hero = (ICombatHero*)caster;
	if (!hero->has_skill(PERK_UNSETTLE))
		return false;

	int element = 0;
	__asm {
		mov ecx, spell_id
		call get_spell_element
		mov element, eax
	}
	return element == SPELL_ELEMENT_EARTH;
}

static float unsettle_reduction_percent(ICombatCreature* target) {
	int hp = target->hit_points();
	if (hp <= 1) return 0.f;
	return 1.f / (float)hp;
}

static void __cdecl DoApplyUnsettle(ICombatUnit* target, int timeline) {
	if (!target || target->unit_kind() != 1)
		return;

	int* stack_ptr = ((GetPtr)target->instance->get_side_ptr_a)((int*)target);

	if (!stack_ptr)
		return;

	void** stack_vt = *(void***)stack_ptr;

	float reduction = unsettle_reduction_percent((ICombatCreature*)target);
	if (reduction > 0.f) {
		((ThiscallVoid_IntPtr_Int_Float)stack_vt[0x4C / sizeof(void*)])(stack_ptr, timeline, reduction);
	}
}

__declspec(naked) void UnsettleTargetedFork() {
	__asm {
		push ebx
		push esi
		push edi
		push eax

		push esi
		push dword ptr [esp + 0x1C]
		call ShouldApplyUnsettle
		add esp, 8
		test al, al
		je UNSETTLE_TARGET_SKIP

		push ebp
		push edi
		call DoApplyUnsettle
		add esp, 8

		UNSETTLE_TARGET_SKIP:
		pop eax
		pop edi
		pop esi
		pop ebx

		mov edx, edi
		mov ecx, esi
		call apply_spell_damage
		jmp[UnsettleTargeted_return]
	}
}

__declspec(naked) void UnsettleAreaFork() {
	__asm {
		push ebx
		push esi
		push edi
		push eax

		push esi
		push dword ptr [esp + 0x1C]
		call ShouldApplyUnsettle
		add esp, 8
		test al, al
		je UNSETTLE_AREA_SKIP

		push ebx
		push ebp
		call DoApplyUnsettle
		add esp, 8

		UNSETTLE_AREA_SKIP:
		pop eax
		pop edi
		pop esi
		pop ebx

		mov edx, ebp
		mov ecx, esi
		call apply_spell_damage
		jmp[UnsettleArea_return]
	}
}

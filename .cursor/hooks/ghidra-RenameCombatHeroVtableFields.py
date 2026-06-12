# Auto-generated from src/structs/CombatUnit.h — do not edit by hand.
# @category H5X
# @menupath Tools.H5X.Rename CombatHero Vtable Fields

from ghidra.program.model.data import PointerDataType
from ghidra.program.model.symbol import SourceType

VTABLE_ADDR = toAddr(0x00E8499C)
STRUCT_NAMES = ("struct_CombatHero_vtable", "CombatUnit_vtable", "CombatHero_vtable")

# (ref_label, offset)
REF_FIELDS = [
    ("ref_CombatHero_GetUnitKind", 0x000),
    ("ref_CombatUnit_ScalarDeletingDtor", 0x004),
    ("ref_CombatUnit_GetOwnerRef", 0x008),
    ("ref_CombatUnit_GetUnitRef", 0x00C),
    ("ref_CombatUnit_NotifyOwner", 0x010),
    ("ref_CombatHero_InitOwnerTimedStats", 0x014),
    ("ref_CombatHero_GetTimedStatsSubobject", 0x018),
    ("ref_CombatHero_ReturnOne", 0x01C),
    ("ref_CombatUnit_GetTimelineField_7C", 0x020),
    ("ref_CombatHero_EnsurePortraitLoaded", 0x024),
    ("ref_CombatUnit_GetActiveBuff", 0x028),
    ("ref_CombatUnit_GetBuffStackState", 0x02C),
    ("ref_CombatUnit_CollectLinkedBuffIds", 0x030),
    ("ref_CombatHero_GetNameWString", 0x034),
    ("ref_CombatUnit_PlayAnimSimple", 0x038),
    ("ref_CombatUnit_GetAnimPosition", 0x03C),
    ("ref_CombatUnit_GetPosition2D", 0x040),
    ("ref_CombatHero_GetOwnerSideKind", 0x044),
    ("ref_CombatHero_ReturnZero", 0x048),
    ("ref_CombatUnit_GetTypeId", 0x04C),
    ("ref_ReturnOne", 0x050),
    ("ref_CombatHero_ReturnZeroB", 0x054),
    ("ref_ReturnFalse", 0x058),
    ("ref_CombatHero_ReturnFalse", 0x05C),
    ("ref_CombatUnit_GetCombatUnitPtr", 0x060),
    ("ref_CombatUnit_GetCombatUnitPtr", 0x064),
    ("ref_CombatHero_NotifyOwnerRefresh", 0x068),
    ("ref_CombatUnit_ReturnZero", 0x06C),
    ("ref_CombatUnit_ReturnZero", 0x070),
    ("ref_CombatHero_GetAuxSubobject", 0x074),
    ("ref_CombatHero_GetAuxSubobject", 0x078),
    ("ref_CombatUnit_ReturnZero", 0x07C),
    ("ref_CombatUnit_ReturnZero", 0x080),
    ("ref_CombatUnit_ReturnZero", 0x084),
    ("ref_CombatUnit_ReturnZero", 0x088),
    ("ref_CombatUnit_ReturnZero", 0x08C),
    ("ref_CombatUnit_ReturnZero", 0x090),
    ("ref_CombatUnit_RefreshUnitData", 0x094),
    ("ref_CombatUnit_OnCombatEvent", 0x098),
    ("ref_CombatHero_ScheduleLinkedSpellFx", 0x09C),
    ("ref_CombatUnit_GetPositionPtr", 0x0A0),
    ("ref_CombatUnit_GetFacing", 0x0A4),
    ("ref_CombatUnit_ComputeAttackAnimPos", 0x0A8),
    ("ref_CombatUnit_OnSiegeAnimHook", 0x0AC),
    ("ref_CombatUnit_GetVisualId", 0x0B0),
    ("ref_CombatUnit_GetAnimDuration", 0x0B4),
    ("ref_CombatHero_Noop", 0x0B8),
    ("ref_CombatUnit_BeginTileQuery", 0x0BC),
    ("ref_CombatUnit_HasTilePair", 0x0C0),
    ("ref_CombatUnit_MarkCurrentTile", 0x0C4),
    ("ref_CombatUnit_MarkTilesFromPosition", 0x0C8),
    ("ref_CombatUnit_TryMoveToTile", 0x0CC),
    ("ref_CombatHero_OnTimelineRefresh", 0x0D0),
    ("ref_CombatHero_NotifyTimelineSub", 0x0D4),
    ("ref_CombatHero_GetSideVisualId", 0x0D8),
    ("ref_CombatUnit_SetNeedsRefresh", 0x0DC),
    ("ref_ReturnFalse", 0x0E0),
    ("ref_CombatUnit_NotifyOwnerVtableE0", 0x0E4),
    ("ref_CombatHero_SetFacingFromTimeline", 0x0E8),
    ("ref_CombatHero_OnSiegeSubhook", 0x0EC),
    ("ref_CombatHero_RetVoid", 0x0F0),
    ("ref_CombatUnit_IsSideZero", 0x0F4),
    ("ref_CombatUnit_GetAnimObject", 0x0F8),
    ("ref_CombatUnit_NotifyTimelineChange", 0x0FC),
    ("ref_CombatUnit_ScheduleBuffAnim", 0x100),
    ("ref_CombatUnit_ScheduleSpellAnim", 0x104),
    ("ref_CombatUnit_ScheduleAttackAnim", 0x108),
    ("ref_CombatUnit_AdvanceTimeline", 0x10C),
    ("ref_CombatUnit_AdvanceTimelineToTarget", 0x110),
    ("ref_CombatHero_ScheduleTimelineHook114", 0x114),
    ("ref_CombatHero_ScheduleTimelineHook118", 0x118),
    ("ref_CombatUnit_ScheduleMovePath", 0x11C),
    ("ref_CombatUnit_ScheduleGenericAnim", 0x120),
    ("ref_CombatUnit_ScheduleRangedAttack", 0x124),
    ("ref_CombatHero_ScheduleTimelineHook128", 0x128),
    ("ref_CombatHero_ScheduleTimelineHook12C", 0x12C),
    ("ref_CombatUnit_ScheduleHitReact", 0x130),
    ("ref_CombatUnit_ScheduleDeathAnim", 0x134),
    ("ref_CombatUnit_ScheduleTimedAnim", 0x138),
    ("ref_CombatHero_ScheduleTimelineHook13C", 0x13C),
    ("ref_CombatUnit_PlayAnimAtTarget", 0x140),
    ("ref_CombatUnit_ScheduleMultiProjectile", 0x144),
    ("ref_CombatUnit_SchedulePhaseAnim", 0x148),
    ("ref_CombatUnit_BeginActionThenNotify", 0x14C),
    ("ref_CombatUnit_ScheduleMoraleAnim", 0x150),
    ("ref_CombatUnit_OnTimelineReached", 0x154),
    ("ref_CombatHero_ScheduleTimelineHook158", 0x158),
    ("ref_CombatUnit_GetTimedStatsPtr", 0x15C),
    ("ref_CombatHero_ScheduleTimelineHook160", 0x160),
    ("ref_CombatHero_ScheduleTimelineHook164", 0x164),
    ("ref_CombatUnit_ScheduleBadLuckAnim", 0x168),
    ("ref_CombatUnit_ScheduleGoodLuckAnim", 0x16C),
    ("ref_CombatUnit_GetMaxTimelineFrame", 0x170),
    ("ref_CombatUnit_GetInitiativeFactor", 0x174),
    ("ref_CombatUnit_OnControlHandoff", 0x178),
    ("ref_CombatUnit_ComputeNextTurnFrame", 0x17C),
    ("ref_CombatUnit_ResetATBToCurrent", 0x180),
    ("ref_CombatUnit_GetATBInfo", 0x184),
    ("ref_CombatUnit_GetATBInfo", 0x188),
    ("ref_SetUnitATB", 0x18C),
    ("ref_ReturnTrue", 0x190),
    ("ref_CombatUnit_GetAnimDebugName", 0x194),
    ("ref_GetHeroAttackCombat", 0x198),
    ("ref_GetHeroDefenseCombat", 0x19C),
    ("ref_CombatHero_ReturnZero", 0x1A0),
    ("ref_CombatHero_GetCombatBonus", 0x1A4),
    ("ref_CombatHero_ReturnZero", 0x1A8),
    ("ref_CombatUnit_GetLuck", 0x1AC),
    ("ref_CombatUnit_GetMorale", 0x1B0),
    ("ref_GetCreatureLuckDebuffContext", 0x1B4),
    ("ref_GetCreatureMoraleDebuffContext", 0x1B8),
    ("ref_GetHeroInitiative", 0x1BC),
    ("ref_CombatHero_ReturnOne", 0x1C0),
    ("ref_CombatUnit_ReturnZero", 0x1C4),
    ("ref_ReturnFalse", 0x1C8),
    ("ref_ReturnFalse", 0x1CC),
    ("ref_CombatHero_OnBuffRefresh", 0x1D0),
    ("ref_CombatHero_ReturnConstant100", 0x1D4),
    ("ref_CombatCreature_GetUnitKind", 0x1D8),
    ("ref_CombatHero_GetStatStubDC", 0x1DC),
    ("ref_CombatHero_GetStatStubE0", 0x1E0),
    ("ref_ReturnFalse", 0x1E4),
    ("ref_CombatHero_RetVoid", 0x1E8),
    ("ref_CombatHero_GetStatStubEC", 0x1EC),
    ("ref_CombatHero_GetStatStubF0", 0x1F0),
    ("ref_CombatHero_GetStatStubF4", 0x1F4),
    ("ref_CombatHero_Noop", 0x1F8),
    ("ref_CombatHero_GetStatStubFC", 0x1FC),
    ("ref_CombatHero_OnSiegeHook200", 0x200),
    ("ref_CombatHero_OnTimelineNotify204", 0x204),
    ("ref_CombatHero_RetVoid", 0x208),
    ("ref_ReturnFalse", 0x20C),
    ("ref_CombatWaitATB", 0x210),
    ("ref_CombatHero_RetVoid", 0x214),
    ("ref_CombatHero_RetVoid", 0x218),
    ("ref_CombatUnit_ReturnZero", 0x21C),
    ("ref_CombatHero_GetManaContext", 0x220),
    ("ref_CombatHero_GetStatStub224", 0x224),
    ("ref_GetHeroCurrentManaCombat", 0x228),
    ("ref_CombatHero_GetManaStub22C", 0x22C),
    ("ref_CombatHero_GetManaStub230", 0x230),
    ("ref_GetHeroCurrentManaCombat", 0x234),
    ("ref_GetHeroStatMaxMana", 0x238),
    ("ref_CombatHero_GetLevel", 0x23C),
    ("ref_CombatUnit_GetSpellMastery", 0x240),
    ("ref_CombatUnit_GetSpellpower", 0x244),
    ("ref_CombatHero_GetSpellbookPtr", 0x248),
    ("ref_GetActiveSpellEffects", 0x24C),
    ("ref_CombatUnit_GetBuffList", 0x250),
    ("ref_CombatUnit_ApplyActiveSpellEffects", 0x254),
    ("ref_CombatUnit_ApplySoulDrainEffects", 0x258),
    ("ref_CombatUnit_ApplySpellEffect4", 0x25C),
    ("ref_CombatUnit_ApplySpellEffect5", 0x260),
    ("ref_CombatUnit_ApplySpellEffect0", 0x264),
    ("ref_CombatUnit_RemoveBuffById", 0x268),
    ("ref_CombatUnit_SetBuffParam", 0x26C),
    ("ref_CombatUnit_RemoveBuffFiltered", 0x270),
    ("ref_CombatUnit_RefreshArtifactBuffs", 0x274),
    ("ref_CombatUnit_PlayMeleeAttackAnim", 0x278),
    ("ref_CombatUnit_PlayRangedAttackAnim", 0x27C),
    ("ref_CombatUnit_PlayAnimDispatch", 0x280),
    ("ref_CombatUnit_PlayAttackAnimAt", 0x284),
    ("ref_CombatUnit_PlayPositionalAnim", 0x288),
    ("ref_CombatUnit_ReturnZero", 0x28C),
    ("ref_HasHeroSkill", 0x290),
    ("ref_HasHeroSpec", 0x294),
    ("ref_CombatUnit_AddSpellLink", 0x298),
    ("ref_CombatUnit_RemoveSpellLinkPair", 0x29C),
    ("ref_CombatUnit_RemoveSpellLinksBySubId", 0x2A0),
    ("ref_CombatUnit_RemoveSpellLinksById", 0x2A4),
    ("ref_CombatUnit_HasSpellLink", 0x2A8),
]

def find_struct():
    dtm = currentProgram.getDataTypeManager()
    for name in STRUCT_NAMES:
        dt = dtm.getDataType('/' + name)
        if dt is not None:
            return dt
    it = dtm.getAllDataTypes()
    while it.hasNext():
        dt = it.next()
        if dt.getName() in STRUCT_NAMES:
            return dt
    return None

struct = find_struct()
if struct is None:
    print("ERROR: struct_CombatHero_vtable not found — create/apply struct first")
else:
    ptr = PointerDataType()
    for ref_label, off in REF_FIELDS:
        comp = struct.getComponentAt(off)
        comment = comp.getComment() if comp is not None else None
        struct.replaceAt(off, ptr, ref_label, comment)
    print("Renamed %d struct components on %s" % (len(REF_FIELDS), struct.getName()))

listing = currentProgram.getListing()
symtab = currentProgram.getSymbolTable()
end = VTABLE_ADDR.add(len(REF_FIELDS) * 4 - 4)
listing.clearCodeUnits(VTABLE_ADDR, end, False)
if struct is not None:
    listing.createData(VTABLE_ADDR, struct)
    print("Re-applied %s at %s" % (struct.getName(), VTABLE_ADDR))

for ref_label, off in REF_FIELDS:
    addr = VTABLE_ADDR.add(off)
    sym = symtab.getPrimarySymbol(addr)
    if sym is None:
        symtab.createLabel(addr, ref_label, SourceType.USER_DEFINED)
    else:
        sym.setName(ref_label, SourceType.USER_DEFINED)

print("Done: %d ref_ pointer labels @ 0x%08X" % (len(REF_FIELDS), 0xE8499C))

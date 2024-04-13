void __init intel_pmu_pebs_data_source_nhm(void)
{
	pebs_data_source[0x05] = OP_LH | P(LVL, L3) | LEVEL(L3) | P(SNOOP, HIT);
	pebs_data_source[0x06] = OP_LH | P(LVL, L3) | LEVEL(L3) | P(SNOOP, HITM);
	pebs_data_source[0x07] = OP_LH | P(LVL, L3) | LEVEL(L3) | P(SNOOP, HITM);
}
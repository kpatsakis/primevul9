void __init intel_pmu_pebs_data_source_skl(bool pmem)
{
	u64 pmem_or_l4 = pmem ? LEVEL(PMEM) : LEVEL(L4);

	pebs_data_source[0x08] = OP_LH | pmem_or_l4 | P(SNOOP, HIT);
	pebs_data_source[0x09] = OP_LH | pmem_or_l4 | REM | P(SNOOP, HIT);
	pebs_data_source[0x0b] = OP_LH | LEVEL(RAM) | REM | P(SNOOP, NONE);
	pebs_data_source[0x0c] = OP_LH | LEVEL(ANY_CACHE) | REM | P(SNOOPX, FWD);
	pebs_data_source[0x0d] = OP_LH | LEVEL(ANY_CACHE) | REM | P(SNOOP, HITM);
}
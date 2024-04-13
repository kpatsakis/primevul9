static int bnx2x_mc_assert(struct bnx2x *bp)
{
	char last_idx;
	int i, j, rc = 0;
	enum storms storm;
	u32 regs[REGS_IN_ENTRY];
	u32 bar_storm_intmem[STORMS_NUM] = {
		BAR_XSTRORM_INTMEM,
		BAR_TSTRORM_INTMEM,
		BAR_CSTRORM_INTMEM,
		BAR_USTRORM_INTMEM
	};
	u32 storm_assert_list_index[STORMS_NUM] = {
		XSTORM_ASSERT_LIST_INDEX_OFFSET,
		TSTORM_ASSERT_LIST_INDEX_OFFSET,
		CSTORM_ASSERT_LIST_INDEX_OFFSET,
		USTORM_ASSERT_LIST_INDEX_OFFSET
	};
	char *storms_string[STORMS_NUM] = {
		"XSTORM",
		"TSTORM",
		"CSTORM",
		"USTORM"
	};

	for (storm = XSTORM; storm < MAX_STORMS; storm++) {
		last_idx = REG_RD8(bp, bar_storm_intmem[storm] +
				   storm_assert_list_index[storm]);
		if (last_idx)
			BNX2X_ERR("%s_ASSERT_LIST_INDEX 0x%x\n",
				  storms_string[storm], last_idx);

		/* print the asserts */
		for (i = 0; i < STROM_ASSERT_ARRAY_SIZE; i++) {
			/* read a single assert entry */
			for (j = 0; j < REGS_IN_ENTRY; j++)
				regs[j] = REG_RD(bp, bar_storm_intmem[storm] +
					  bnx2x_get_assert_list_entry(bp,
								      storm,
								      i) +
					  sizeof(u32) * j);

			/* log entry if it contains a valid assert */
			if (regs[0] != COMMON_ASM_INVALID_ASSERT_OPCODE) {
				BNX2X_ERR("%s_ASSERT_INDEX 0x%x = 0x%08x 0x%08x 0x%08x 0x%08x\n",
					  storms_string[storm], i, regs[3],
					  regs[2], regs[1], regs[0]);
				rc++;
			} else {
				break;
			}
		}
	}

	BNX2X_ERR("Chip Revision: %s, FW Version: %d_%d_%d\n",
		  CHIP_IS_E1(bp) ? "everest1" :
		  CHIP_IS_E1H(bp) ? "everest1h" :
		  CHIP_IS_E2(bp) ? "everest2" : "everest3",
		  BCM_5710_FW_MAJOR_VERSION,
		  BCM_5710_FW_MINOR_VERSION,
		  BCM_5710_FW_REVISION_VERSION);

	return rc;
}
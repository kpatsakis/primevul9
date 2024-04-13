static void bnx2x_init_internal_common(struct bnx2x *bp)
{
	int i;

	/* Zero this manually as its initialization is
	   currently missing in the initTool */
	for (i = 0; i < (USTORM_AGG_DATA_SIZE >> 2); i++)
		REG_WR(bp, BAR_USTRORM_INTMEM +
		       USTORM_AGG_DATA_OFFSET + i * 4, 0);
	if (!CHIP_IS_E1x(bp)) {
		REG_WR8(bp, BAR_CSTRORM_INTMEM + CSTORM_IGU_MODE_OFFSET,
			CHIP_INT_MODE_IS_BC(bp) ?
			HC_IGU_BC_MODE : HC_IGU_NBC_MODE);
	}
}
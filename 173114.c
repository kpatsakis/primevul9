static void bnx2x_reset_common(struct bnx2x *bp)
{
	u32 val = 0x1400;

	/* reset_common */
	REG_WR(bp, GRCBASE_MISC + MISC_REGISTERS_RESET_REG_1_CLEAR,
	       0xd3ffff7f);

	if (CHIP_IS_E3(bp)) {
		val |= MISC_REGISTERS_RESET_REG_2_MSTAT0;
		val |= MISC_REGISTERS_RESET_REG_2_MSTAT1;
	}

	REG_WR(bp, GRCBASE_MISC + MISC_REGISTERS_RESET_REG_2_CLEAR, val);
}
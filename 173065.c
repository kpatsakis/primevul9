static void bnx2x_set_reset_done(struct bnx2x *bp)
{
	u32 val;
	u32 bit = BP_PATH(bp) ?
		BNX2X_PATH1_RST_IN_PROG_BIT : BNX2X_PATH0_RST_IN_PROG_BIT;
	bnx2x_acquire_hw_lock(bp, HW_LOCK_RESOURCE_RECOVERY_REG);
	val = REG_RD(bp, BNX2X_RECOVERY_GLOB_REG);

	/* Clear the bit */
	val &= ~bit;
	REG_WR(bp, BNX2X_RECOVERY_GLOB_REG, val);

	bnx2x_release_hw_lock(bp, HW_LOCK_RESOURCE_RECOVERY_REG);
}
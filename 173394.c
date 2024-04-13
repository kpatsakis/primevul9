static void bnx2x_clear_reset_global(struct bnx2x *bp)
{
	u32 val;
	bnx2x_acquire_hw_lock(bp, HW_LOCK_RESOURCE_RECOVERY_REG);
	val = REG_RD(bp, BNX2X_RECOVERY_GLOB_REG);
	REG_WR(bp, BNX2X_RECOVERY_GLOB_REG, val & (~BNX2X_GLOBAL_RESET_BIT));
	bnx2x_release_hw_lock(bp, HW_LOCK_RESOURCE_RECOVERY_REG);
}
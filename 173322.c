void bnx2x_set_pf_load(struct bnx2x *bp)
{
	u32 val1, val;
	u32 mask = BP_PATH(bp) ? BNX2X_PATH1_LOAD_CNT_MASK :
			     BNX2X_PATH0_LOAD_CNT_MASK;
	u32 shift = BP_PATH(bp) ? BNX2X_PATH1_LOAD_CNT_SHIFT :
			     BNX2X_PATH0_LOAD_CNT_SHIFT;

	bnx2x_acquire_hw_lock(bp, HW_LOCK_RESOURCE_RECOVERY_REG);
	val = REG_RD(bp, BNX2X_RECOVERY_GLOB_REG);

	DP(NETIF_MSG_IFUP, "Old GEN_REG_VAL=0x%08x\n", val);

	/* get the current counter value */
	val1 = (val & mask) >> shift;

	/* set bit of that PF */
	val1 |= (1 << bp->pf_num);

	/* clear the old value */
	val &= ~mask;

	/* set the new one */
	val |= ((val1 << shift) & mask);

	REG_WR(bp, BNX2X_RECOVERY_GLOB_REG, val);
	bnx2x_release_hw_lock(bp, HW_LOCK_RESOURCE_RECOVERY_REG);
}
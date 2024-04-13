static bool bnx2x_reset_is_global(struct bnx2x *bp)
{
	u32 val = REG_RD(bp, BNX2X_RECOVERY_GLOB_REG);

	DP(NETIF_MSG_HW, "GEN_REG_VAL=0x%08x\n", val);
	return (val & BNX2X_GLOBAL_RESET_BIT) ? true : false;
}
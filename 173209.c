static bool bnx2x_get_load_status(struct bnx2x *bp, int engine)
{
	u32 mask = (engine ? BNX2X_PATH1_LOAD_CNT_MASK :
			     BNX2X_PATH0_LOAD_CNT_MASK);
	u32 shift = (engine ? BNX2X_PATH1_LOAD_CNT_SHIFT :
			     BNX2X_PATH0_LOAD_CNT_SHIFT);
	u32 val = REG_RD(bp, BNX2X_RECOVERY_GLOB_REG);

	DP(NETIF_MSG_HW | NETIF_MSG_IFUP, "GLOB_REG=0x%08x\n", val);

	val = (val & mask) >> shift;

	DP(NETIF_MSG_HW | NETIF_MSG_IFUP, "load mask for engine %d = 0x%x\n",
	   engine, val);

	return val != 0;
}
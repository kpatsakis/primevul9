static void bnx2x_init_dropless_fc(struct bnx2x *bp)
{
	u32 pause_enabled = 0;

	if (!CHIP_IS_E1(bp) && bp->dropless_fc && bp->link_vars.link_up) {
		if (bp->link_vars.flow_ctrl & BNX2X_FLOW_CTRL_TX)
			pause_enabled = 1;

		REG_WR(bp, BAR_USTRORM_INTMEM +
			   USTORM_ETH_PAUSE_ENABLED_OFFSET(BP_PORT(bp)),
		       pause_enabled);
	}

	DP(NETIF_MSG_IFUP | NETIF_MSG_LINK, "dropless_fc is %s\n",
	   pause_enabled ? "enabled" : "disabled");
}
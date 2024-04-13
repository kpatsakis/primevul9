static void bnx2x_igu_int_disable(struct bnx2x *bp)
{
	u32 val = REG_RD(bp, IGU_REG_PF_CONFIGURATION);

	val &= ~(IGU_PF_CONF_MSI_MSIX_EN |
		 IGU_PF_CONF_INT_LINE_EN |
		 IGU_PF_CONF_ATTN_BIT_EN);

	DP(NETIF_MSG_IFDOWN, "write %x to IGU\n", val);

	/* flush all outstanding writes */
	mmiowb();

	REG_WR(bp, IGU_REG_PF_CONFIGURATION, val);
	if (REG_RD(bp, IGU_REG_PF_CONFIGURATION) != val)
		BNX2X_ERR("BUG! Proper val not read from IGU!\n");
}
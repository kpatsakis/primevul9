static void bnx2x_hc_int_disable(struct bnx2x *bp)
{
	int port = BP_PORT(bp);
	u32 addr = port ? HC_REG_CONFIG_1 : HC_REG_CONFIG_0;
	u32 val = REG_RD(bp, addr);

	/* in E1 we must use only PCI configuration space to disable
	 * MSI/MSIX capability
	 * It's forbidden to disable IGU_PF_CONF_MSI_MSIX_EN in HC block
	 */
	if (CHIP_IS_E1(bp)) {
		/* Since IGU_PF_CONF_MSI_MSIX_EN still always on
		 * Use mask register to prevent from HC sending interrupts
		 * after we exit the function
		 */
		REG_WR(bp, HC_REG_INT_MASK + port*4, 0);

		val &= ~(HC_CONFIG_0_REG_SINGLE_ISR_EN_0 |
			 HC_CONFIG_0_REG_INT_LINE_EN_0 |
			 HC_CONFIG_0_REG_ATTN_BIT_EN_0);
	} else
		val &= ~(HC_CONFIG_0_REG_SINGLE_ISR_EN_0 |
			 HC_CONFIG_0_REG_MSI_MSIX_INT_EN_0 |
			 HC_CONFIG_0_REG_INT_LINE_EN_0 |
			 HC_CONFIG_0_REG_ATTN_BIT_EN_0);

	DP(NETIF_MSG_IFDOWN,
	   "write %x to HC %d (addr 0x%x)\n",
	   val, port, addr);

	/* flush all outstanding writes */
	mmiowb();

	REG_WR(bp, addr, val);
	if (REG_RD(bp, addr) != val)
		BNX2X_ERR("BUG! Proper val not read from IGU!\n");
}
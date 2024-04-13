static void xemaclite_enable_interrupts(struct net_local *drvdata)
{
	u32 reg_data;

	/* Enable the Tx interrupts for the first Buffer */
	reg_data = xemaclite_readl(drvdata->base_addr + XEL_TSR_OFFSET);
	xemaclite_writel(reg_data | XEL_TSR_XMIT_IE_MASK,
			 drvdata->base_addr + XEL_TSR_OFFSET);

	/* Enable the Rx interrupts for the first buffer */
	xemaclite_writel(XEL_RSR_RECV_IE_MASK, drvdata->base_addr + XEL_RSR_OFFSET);

	/* Enable the Global Interrupt Enable */
	xemaclite_writel(XEL_GIER_GIE_MASK, drvdata->base_addr + XEL_GIER_OFFSET);
}
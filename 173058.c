static void bnx2x_igu_int_enable(struct bnx2x *bp)
{
	u32 val;
	bool msix = (bp->flags & USING_MSIX_FLAG) ? true : false;
	bool single_msix = (bp->flags & USING_SINGLE_MSIX_FLAG) ? true : false;
	bool msi = (bp->flags & USING_MSI_FLAG) ? true : false;

	val = REG_RD(bp, IGU_REG_PF_CONFIGURATION);

	if (msix) {
		val &= ~(IGU_PF_CONF_INT_LINE_EN |
			 IGU_PF_CONF_SINGLE_ISR_EN);
		val |= (IGU_PF_CONF_MSI_MSIX_EN |
			IGU_PF_CONF_ATTN_BIT_EN);

		if (single_msix)
			val |= IGU_PF_CONF_SINGLE_ISR_EN;
	} else if (msi) {
		val &= ~IGU_PF_CONF_INT_LINE_EN;
		val |= (IGU_PF_CONF_MSI_MSIX_EN |
			IGU_PF_CONF_ATTN_BIT_EN |
			IGU_PF_CONF_SINGLE_ISR_EN);
	} else {
		val &= ~IGU_PF_CONF_MSI_MSIX_EN;
		val |= (IGU_PF_CONF_INT_LINE_EN |
			IGU_PF_CONF_ATTN_BIT_EN |
			IGU_PF_CONF_SINGLE_ISR_EN);
	}

	/* Clean previous status - need to configure igu prior to ack*/
	if ((!msix) || single_msix) {
		REG_WR(bp, IGU_REG_PF_CONFIGURATION, val);
		bnx2x_ack_int(bp);
	}

	val |= IGU_PF_CONF_FUNC_EN;

	DP(NETIF_MSG_IFUP, "write 0x%x to IGU  mode %s\n",
	   val, (msix ? "MSI-X" : (msi ? "MSI" : "INTx")));

	REG_WR(bp, IGU_REG_PF_CONFIGURATION, val);

	if (val & IGU_PF_CONF_INT_LINE_EN)
		pci_intx(bp->pdev, true);

	barrier();

	/* init leading/trailing edge */
	if (IS_MF(bp)) {
		val = (0xee0f | (1 << (BP_VN(bp) + 4)));
		if (bp->port.pmf)
			/* enable nig and gpio3 attention */
			val |= 0x1100;
	} else
		val = 0xffff;

	REG_WR(bp, IGU_REG_TRAILING_EDGE_LATCH, val);
	REG_WR(bp, IGU_REG_LEADING_EDGE_LATCH, val);

	/* Make sure that interrupts are indeed enabled from here on */
	mmiowb();
}
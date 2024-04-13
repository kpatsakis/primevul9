static void xemaclite_update_address(struct net_local *drvdata,
				     u8 *address_ptr)
{
	void __iomem *addr;
	u32 reg_data;

	/* Determine the expected Tx buffer address */
	addr = drvdata->base_addr + drvdata->next_tx_buf_to_use;

	xemaclite_aligned_write(address_ptr, (u32 __force *)addr, ETH_ALEN);

	xemaclite_writel(ETH_ALEN, addr + XEL_TPLR_OFFSET);

	/* Update the MAC address in the EmacLite */
	reg_data = xemaclite_readl(addr + XEL_TSR_OFFSET);
	xemaclite_writel(reg_data | XEL_TSR_PROG_MAC_ADDR, addr + XEL_TSR_OFFSET);

	/* Wait for EmacLite to finish with the MAC address update */
	while ((xemaclite_readl(addr + XEL_TSR_OFFSET) &
		XEL_TSR_PROG_MAC_ADDR) != 0)
		;
}
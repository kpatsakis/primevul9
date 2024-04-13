mwifiex_pcie_send_boot_cmd(struct mwifiex_adapter *adapter, struct sk_buff *skb)
{
	dma_addr_t buf_pa;
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;

	if (!(skb->data && skb->len)) {
		mwifiex_dbg(adapter, ERROR,
			    "Invalid parameter in %s <%p. len %d>\n",
			    __func__, skb->data, skb->len);
		return -1;
	}

	if (mwifiex_map_pci_memory(adapter, skb, skb->len, PCI_DMA_TODEVICE))
		return -1;

	buf_pa = MWIFIEX_SKB_DMA_ADDR(skb);

	/* Write the lower 32bits of the physical address to low command
	 * address scratch register
	 */
	if (mwifiex_write_reg(adapter, reg->cmd_addr_lo, (u32)buf_pa)) {
		mwifiex_dbg(adapter, ERROR,
			    "%s: failed to write download command to boot code.\n",
			    __func__);
		mwifiex_unmap_pci_memory(adapter, skb, PCI_DMA_TODEVICE);
		return -1;
	}

	/* Write the upper 32bits of the physical address to high command
	 * address scratch register
	 */
	if (mwifiex_write_reg(adapter, reg->cmd_addr_hi,
			      (u32)((u64)buf_pa >> 32))) {
		mwifiex_dbg(adapter, ERROR,
			    "%s: failed to write download command to boot code.\n",
			    __func__);
		mwifiex_unmap_pci_memory(adapter, skb, PCI_DMA_TODEVICE);
		return -1;
	}

	/* Write the command length to cmd_size scratch register */
	if (mwifiex_write_reg(adapter, reg->cmd_size, skb->len)) {
		mwifiex_dbg(adapter, ERROR,
			    "%s: failed to write command len to cmd_size scratch reg\n",
			    __func__);
		mwifiex_unmap_pci_memory(adapter, skb, PCI_DMA_TODEVICE);
		return -1;
	}

	/* Ring the door bell */
	if (mwifiex_write_reg(adapter, PCIE_CPU_INT_EVENT,
			      CPU_INTR_DOOR_BELL)) {
		mwifiex_dbg(adapter, ERROR,
			    "%s: failed to assert door-bell intr\n", __func__);
		mwifiex_unmap_pci_memory(adapter, skb, PCI_DMA_TODEVICE);
		return -1;
	}

	return 0;
}
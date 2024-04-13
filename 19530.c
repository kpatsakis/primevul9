static void iwl_pcie_alloc_fw_monitor_block(struct iwl_trans *trans,
					    u8 max_power, u8 min_power)
{
	void *cpu_addr = NULL;
	dma_addr_t phys = 0;
	u32 size = 0;
	u8 power;

	for (power = max_power; power >= min_power; power--) {
		size = BIT(power);
		cpu_addr = dma_alloc_coherent(trans->dev, size, &phys,
					      GFP_KERNEL | __GFP_NOWARN);
		if (!cpu_addr)
			continue;

		IWL_INFO(trans,
			 "Allocated 0x%08x bytes for firmware monitor.\n",
			 size);
		break;
	}

	if (WARN_ON_ONCE(!cpu_addr))
		return;

	if (power != max_power)
		IWL_ERR(trans,
			"Sorry - debug buffer is only %luK while you requested %luK\n",
			(unsigned long)BIT(power - 10),
			(unsigned long)BIT(max_power - 10));

	trans->dbg.fw_mon[trans->dbg.num_blocks].block = cpu_addr;
	trans->dbg.fw_mon[trans->dbg.num_blocks].physical = phys;
	trans->dbg.fw_mon[trans->dbg.num_blocks].size = size;
	trans->dbg.num_blocks++;
}
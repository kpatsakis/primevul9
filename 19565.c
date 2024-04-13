static void iwl_pcie_free_fw_monitor(struct iwl_trans *trans)
{
	int i;

	for (i = 0; i < trans->dbg.num_blocks; i++) {
		dma_free_coherent(trans->dev, trans->dbg.fw_mon[i].size,
				  trans->dbg.fw_mon[i].block,
				  trans->dbg.fw_mon[i].physical);
		trans->dbg.fw_mon[i].block = NULL;
		trans->dbg.fw_mon[i].physical = 0;
		trans->dbg.fw_mon[i].size = 0;
		trans->dbg.num_blocks--;
	}
}
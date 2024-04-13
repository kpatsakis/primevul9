void iwl_pcie_apm_stop_master(struct iwl_trans *trans)
{
	int ret;

	/* stop device's busmaster DMA activity */
	iwl_set_bit(trans, trans->trans_cfg->csr->addr_sw_reset,
		    BIT(trans->trans_cfg->csr->flag_stop_master));

	ret = iwl_poll_bit(trans, trans->trans_cfg->csr->addr_sw_reset,
			   BIT(trans->trans_cfg->csr->flag_master_dis),
			   BIT(trans->trans_cfg->csr->flag_master_dis), 100);
	if (ret < 0)
		IWL_WARN(trans, "Master Disable Timed Out, 100 usec\n");

	IWL_DEBUG_INFO(trans, "stop master\n");
}
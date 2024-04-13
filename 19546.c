static void iwl_trans_pcie_configure(struct iwl_trans *trans,
				     const struct iwl_trans_config *trans_cfg)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);

	trans_pcie->cmd_queue = trans_cfg->cmd_queue;
	trans_pcie->cmd_fifo = trans_cfg->cmd_fifo;
	trans_pcie->cmd_q_wdg_timeout = trans_cfg->cmd_q_wdg_timeout;
	if (WARN_ON(trans_cfg->n_no_reclaim_cmds > MAX_NO_RECLAIM_CMDS))
		trans_pcie->n_no_reclaim_cmds = 0;
	else
		trans_pcie->n_no_reclaim_cmds = trans_cfg->n_no_reclaim_cmds;
	if (trans_pcie->n_no_reclaim_cmds)
		memcpy(trans_pcie->no_reclaim_cmds, trans_cfg->no_reclaim_cmds,
		       trans_pcie->n_no_reclaim_cmds * sizeof(u8));

	trans_pcie->rx_buf_size = trans_cfg->rx_buf_size;
	trans_pcie->rx_page_order =
		iwl_trans_get_rb_size_order(trans_pcie->rx_buf_size);

	trans_pcie->bc_table_dword = trans_cfg->bc_table_dword;
	trans_pcie->scd_set_active = trans_cfg->scd_set_active;
	trans_pcie->sw_csum_tx = trans_cfg->sw_csum_tx;

	trans_pcie->page_offs = trans_cfg->cb_data_offs;
	trans_pcie->dev_cmd_offs = trans_cfg->cb_data_offs + sizeof(void *);

	trans->command_groups = trans_cfg->command_groups;
	trans->command_groups_size = trans_cfg->command_groups_size;

	/* Initialize NAPI here - it should be before registering to mac80211
	 * in the opmode but after the HW struct is allocated.
	 * As this function may be called again in some corner cases don't
	 * do anything if NAPI was already initialized.
	 */
	if (trans_pcie->napi_dev.reg_state != NETREG_DUMMY)
		init_dummy_netdev(&trans_pcie->napi_dev);
}
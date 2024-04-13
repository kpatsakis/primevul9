static void iwl_trans_pcie_sw_reset(struct iwl_trans *trans)
{
	/* Reset entire device - do controller reset (results in SHRD_HW_RST) */
	iwl_set_bit(trans, trans->trans_cfg->csr->addr_sw_reset,
		    BIT(trans->trans_cfg->csr->flag_sw_reset));
	usleep_range(5000, 6000);
}
void iwl_pcie_d3_complete_suspend(struct iwl_trans *trans,
				  bool test, bool reset)
{
	iwl_disable_interrupts(trans);

	/*
	 * in testing mode, the host stays awake and the
	 * hardware won't be reset (not even partially)
	 */
	if (test)
		return;

	iwl_pcie_disable_ict(trans);

	iwl_pcie_synchronize_irqs(trans);

	iwl_clear_bit(trans, CSR_GP_CNTRL,
		      BIT(trans->trans_cfg->csr->flag_mac_access_req));
	iwl_clear_bit(trans, CSR_GP_CNTRL,
		      BIT(trans->trans_cfg->csr->flag_init_done));

	if (reset) {
		/*
		 * reset TX queues -- some of their registers reset during S3
		 * so if we don't reset everything here the D3 image would try
		 * to execute some invalid memory upon resume
		 */
		iwl_trans_pcie_tx_reset(trans);
	}

	iwl_pcie_set_pwr(trans, true);
}
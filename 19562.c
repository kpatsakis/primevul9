static int iwl_pcie_nic_init(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	int ret;

	/* nic_init */
	spin_lock(&trans_pcie->irq_lock);
	ret = iwl_pcie_apm_init(trans);
	spin_unlock(&trans_pcie->irq_lock);

	if (ret)
		return ret;

	iwl_pcie_set_pwr(trans, false);

	iwl_op_mode_nic_config(trans->op_mode);

	/* Allocate the RX queue, or reset if it is already allocated */
	iwl_pcie_rx_init(trans);

	/* Allocate or reset and init all Tx and Command queues */
	if (iwl_pcie_tx_init(trans))
		return -ENOMEM;

	if (trans->trans_cfg->base_params->shadow_reg_enable) {
		/* enable shadow regs in HW */
		iwl_set_bit(trans, CSR_MAC_SHADOW_REG_CTRL, 0x800FFFFF);
		IWL_DEBUG_INFO(trans, "Enabling shadow registers in device\n");
	}

	return 0;
}
static int iwl_trans_pcie_d3_suspend(struct iwl_trans *trans, bool test,
				     bool reset)
{
	int ret;
	struct iwl_trans_pcie *trans_pcie =  IWL_TRANS_GET_PCIE_TRANS(trans);

	/*
	 * Family IWL_DEVICE_FAMILY_AX210 and above persist mode is set by FW.
	 */
	if (!reset && trans->trans_cfg->device_family < IWL_DEVICE_FAMILY_AX210) {
		/* Enable persistence mode to avoid reset */
		iwl_set_bit(trans, CSR_HW_IF_CONFIG_REG,
			    CSR_HW_IF_CONFIG_REG_PERSIST_MODE);
	}

	if (trans->trans_cfg->device_family >= IWL_DEVICE_FAMILY_AX210) {
		iwl_write_umac_prph(trans, UREG_DOORBELL_TO_ISR6,
				    UREG_DOORBELL_TO_ISR6_SUSPEND);

		ret = wait_event_timeout(trans_pcie->sx_waitq,
					 trans_pcie->sx_complete, 2 * HZ);
		/*
		 * Invalidate it toward resume.
		 */
		trans_pcie->sx_complete = false;

		if (!ret) {
			IWL_ERR(trans, "Timeout entering D3\n");
			return -ETIMEDOUT;
		}
	}
	iwl_pcie_d3_complete_suspend(trans, test, reset);

	return 0;
}
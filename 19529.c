static int _iwl_trans_pcie_start_hw(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	int err;

	lockdep_assert_held(&trans_pcie->mutex);

	err = iwl_pcie_prepare_card_hw(trans);
	if (err) {
		IWL_ERR(trans, "Error while preparing HW: %d\n", err);
		return err;
	}

	err = iwl_trans_pcie_clear_persistence_bit(trans);
	if (err)
		return err;

	iwl_trans_pcie_sw_reset(trans);

	err = iwl_pcie_apm_init(trans);
	if (err)
		return err;

	iwl_pcie_init_msix(trans_pcie);

	/* From now on, the op_mode will be kept updated about RF kill state */
	iwl_enable_rfkill_int(trans);

	trans_pcie->opmode_down = false;

	/* Set is_down to false here so that...*/
	trans_pcie->is_down = false;

	/* ...rfkill can call stop_device and set it false if needed */
	iwl_pcie_check_hw_rf_kill(trans);

	return 0;
}
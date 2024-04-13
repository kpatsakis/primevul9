void iwl_trans_pcie_rf_kill(struct iwl_trans *trans, bool state)
{
	struct iwl_trans_pcie __maybe_unused *trans_pcie =
		IWL_TRANS_GET_PCIE_TRANS(trans);

	lockdep_assert_held(&trans_pcie->mutex);

	IWL_WARN(trans, "reporting RF_KILL (radio %s)\n",
		 state ? "disabled" : "enabled");
	if (iwl_op_mode_hw_rf_kill(trans->op_mode, state)) {
		if (trans->trans_cfg->gen2)
			_iwl_trans_pcie_gen2_stop_device(trans);
		else
			_iwl_trans_pcie_stop_device(trans);
	}
}
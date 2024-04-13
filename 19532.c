bool iwl_pcie_check_hw_rf_kill(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie =  IWL_TRANS_GET_PCIE_TRANS(trans);
	bool hw_rfkill = iwl_is_rfkill_set(trans);
	bool prev = test_bit(STATUS_RFKILL_OPMODE, &trans->status);
	bool report;

	if (hw_rfkill) {
		set_bit(STATUS_RFKILL_HW, &trans->status);
		set_bit(STATUS_RFKILL_OPMODE, &trans->status);
	} else {
		clear_bit(STATUS_RFKILL_HW, &trans->status);
		if (trans_pcie->opmode_down)
			clear_bit(STATUS_RFKILL_OPMODE, &trans->status);
	}

	report = test_bit(STATUS_RFKILL_OPMODE, &trans->status);

	if (prev != report)
		iwl_trans_pcie_rf_kill(trans, report);

	return hw_rfkill;
}
static void iwl_trans_pcie_stop_device(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	bool was_in_rfkill;

	mutex_lock(&trans_pcie->mutex);
	trans_pcie->opmode_down = true;
	was_in_rfkill = test_bit(STATUS_RFKILL_OPMODE, &trans->status);
	_iwl_trans_pcie_stop_device(trans);
	iwl_trans_pcie_handle_stop_rfkill(trans, was_in_rfkill);
	mutex_unlock(&trans_pcie->mutex);
}
void iwl_trans_pcie_handle_stop_rfkill(struct iwl_trans *trans,
				       bool was_in_rfkill)
{
	bool hw_rfkill;

	/*
	 * Check again since the RF kill state may have changed while
	 * all the interrupts were disabled, in this case we couldn't
	 * receive the RF kill interrupt and update the state in the
	 * op_mode.
	 * Don't call the op_mode if the rkfill state hasn't changed.
	 * This allows the op_mode to call stop_device from the rfkill
	 * notification without endless recursion. Under very rare
	 * circumstances, we might have a small recursion if the rfkill
	 * state changed exactly now while we were called from stop_device.
	 * This is very unlikely but can happen and is supported.
	 */
	hw_rfkill = iwl_is_rfkill_set(trans);
	if (hw_rfkill) {
		set_bit(STATUS_RFKILL_HW, &trans->status);
		set_bit(STATUS_RFKILL_OPMODE, &trans->status);
	} else {
		clear_bit(STATUS_RFKILL_HW, &trans->status);
		clear_bit(STATUS_RFKILL_OPMODE, &trans->status);
	}
	if (hw_rfkill != was_in_rfkill)
		iwl_trans_pcie_rf_kill(trans, hw_rfkill);
}
static inline int bnx2x_func_switch_update(struct bnx2x *bp, int suspend)
{
	int rc;
	struct bnx2x_func_state_params func_params = {NULL};
	struct bnx2x_func_switch_update_params *switch_update_params =
		&func_params.params.switch_update;

	/* Prepare parameters for function state transitions */
	__set_bit(RAMROD_COMP_WAIT, &func_params.ramrod_flags);
	__set_bit(RAMROD_RETRY, &func_params.ramrod_flags);

	func_params.f_obj = &bp->func_obj;
	func_params.cmd = BNX2X_F_CMD_SWITCH_UPDATE;

	/* Function parameters */
	__set_bit(BNX2X_F_UPDATE_TX_SWITCH_SUSPEND_CHNG,
		  &switch_update_params->changes);
	if (suspend)
		__set_bit(BNX2X_F_UPDATE_TX_SWITCH_SUSPEND,
			  &switch_update_params->changes);

	rc = bnx2x_func_state_change(bp, &func_params);

	return rc;
}
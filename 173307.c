static int bnx2x_send_reset_timesync_ramrod(struct bnx2x *bp)
{
	struct bnx2x_func_state_params func_params = {NULL};
	struct bnx2x_func_set_timesync_params *set_timesync_params =
		&func_params.params.set_timesync;

	/* Prepare parameters for function state transitions */
	__set_bit(RAMROD_COMP_WAIT, &func_params.ramrod_flags);
	__set_bit(RAMROD_RETRY, &func_params.ramrod_flags);

	func_params.f_obj = &bp->func_obj;
	func_params.cmd = BNX2X_F_CMD_SET_TIMESYNC;

	/* Function parameters */
	set_timesync_params->drift_adjust_cmd = TS_DRIFT_ADJUST_RESET;
	set_timesync_params->offset_cmd = TS_OFFSET_KEEP;

	return bnx2x_func_state_change(bp, &func_params);
}
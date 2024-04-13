static int bnx2x_send_update_drift_ramrod(struct bnx2x *bp, int drift_dir,
					  int best_val, int best_period)
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
	set_timesync_params->drift_adjust_cmd = TS_DRIFT_ADJUST_SET;
	set_timesync_params->offset_cmd = TS_OFFSET_KEEP;
	set_timesync_params->add_sub_drift_adjust_value =
		drift_dir ? TS_ADD_VALUE : TS_SUB_VALUE;
	set_timesync_params->drift_adjust_value = best_val;
	set_timesync_params->drift_adjust_period = best_period;

	return bnx2x_func_state_change(bp, &func_params);
}
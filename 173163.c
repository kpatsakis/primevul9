static int bnx2x_reset_hw(struct bnx2x *bp, u32 load_code)
{
	struct bnx2x_func_state_params func_params = {NULL};

	/* Prepare parameters for function state transitions */
	__set_bit(RAMROD_COMP_WAIT, &func_params.ramrod_flags);

	func_params.f_obj = &bp->func_obj;
	func_params.cmd = BNX2X_F_CMD_HW_RESET;

	func_params.params.hw_init.load_phase = load_code;

	return bnx2x_func_state_change(bp, &func_params);
}
static void sdma_set_state(struct sdma_engine *sde,
			   enum sdma_states next_state)
{
	struct sdma_state *ss = &sde->state;
	const struct sdma_set_state_action *action = sdma_action_table;
	unsigned op = 0;

	trace_hfi1_sdma_state(
		sde,
		sdma_state_names[ss->current_state],
		sdma_state_names[next_state]);

	/* debugging bookkeeping */
	ss->previous_state = ss->current_state;
	ss->previous_op = ss->current_op;
	ss->current_state = next_state;

	if (ss->previous_state != sdma_state_s99_running &&
	    next_state == sdma_state_s99_running)
		sdma_flush(sde);

	if (action[next_state].op_enable)
		op |= SDMA_SENDCTRL_OP_ENABLE;

	if (action[next_state].op_intenable)
		op |= SDMA_SENDCTRL_OP_INTENABLE;

	if (action[next_state].op_halt)
		op |= SDMA_SENDCTRL_OP_HALT;

	if (action[next_state].op_cleanup)
		op |= SDMA_SENDCTRL_OP_CLEANUP;

	if (action[next_state].go_s99_running_tofalse)
		ss->go_s99_running = 0;

	if (action[next_state].go_s99_running_totrue)
		ss->go_s99_running = 1;

	ss->current_op = op;
	sdma_sendctrl(sde, ss->current_op);
}
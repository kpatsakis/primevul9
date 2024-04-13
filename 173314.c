int bnx2x_send_final_clnup(struct bnx2x *bp, u8 clnup_func, u32 poll_cnt)
{
	u32 op_gen_command = 0;
	u32 comp_addr = BAR_CSTRORM_INTMEM +
			CSTORM_FINAL_CLEANUP_COMPLETE_OFFSET(clnup_func);
	int ret = 0;

	if (REG_RD(bp, comp_addr)) {
		BNX2X_ERR("Cleanup complete was not 0 before sending\n");
		return 1;
	}

	op_gen_command |= OP_GEN_PARAM(XSTORM_AGG_INT_FINAL_CLEANUP_INDEX);
	op_gen_command |= OP_GEN_TYPE(XSTORM_AGG_INT_FINAL_CLEANUP_COMP_TYPE);
	op_gen_command |= OP_GEN_AGG_VECT(clnup_func);
	op_gen_command |= 1 << SDM_OP_GEN_AGG_VECT_IDX_VALID_SHIFT;

	DP(BNX2X_MSG_SP, "sending FW Final cleanup\n");
	REG_WR(bp, XSDM_REG_OPERATION_GEN, op_gen_command);

	if (bnx2x_flr_clnup_reg_poll(bp, comp_addr, 1, poll_cnt) != 1) {
		BNX2X_ERR("FW final cleanup did not succeed\n");
		DP(BNX2X_MSG_SP, "At timeout completion address contained %x\n",
		   (REG_RD(bp, comp_addr)));
		bnx2x_panic();
		return 1;
	}
	/* Zero completion for next FLR */
	REG_WR(bp, comp_addr, 0);

	return ret;
}
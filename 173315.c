static void bnx2x_pbf_pN_cmd_flushed(struct bnx2x *bp,
				     struct pbf_pN_cmd_regs *regs,
				     u32 poll_count)
{
	u32 occup, to_free, freed, freed_start;
	u32 cur_cnt = poll_count;

	occup = to_free = REG_RD(bp, regs->lines_occup);
	freed = freed_start = REG_RD(bp, regs->lines_freed);

	DP(BNX2X_MSG_SP, "OCCUPANCY[%d]   : s:%x\n", regs->pN, occup);
	DP(BNX2X_MSG_SP, "LINES_FREED[%d] : s:%x\n", regs->pN, freed);

	while (occup && ((u32)SUB_S32(freed, freed_start) < to_free)) {
		if (cur_cnt--) {
			udelay(FLR_WAIT_INTERVAL);
			occup = REG_RD(bp, regs->lines_occup);
			freed = REG_RD(bp, regs->lines_freed);
		} else {
			DP(BNX2X_MSG_SP, "PBF cmd queue[%d] timed out\n",
			   regs->pN);
			DP(BNX2X_MSG_SP, "OCCUPANCY[%d]   : s:%x\n",
			   regs->pN, occup);
			DP(BNX2X_MSG_SP, "LINES_FREED[%d] : s:%x\n",
			   regs->pN, freed);
			break;
		}
	}
	DP(BNX2X_MSG_SP, "Waited %d*%d usec for PBF cmd queue[%d]\n",
	   poll_count-cur_cnt, FLR_WAIT_INTERVAL, regs->pN);
}
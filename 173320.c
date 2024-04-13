static void bnx2x_pbf_pN_buf_flushed(struct bnx2x *bp,
				     struct pbf_pN_buf_regs *regs,
				     u32 poll_count)
{
	u32 init_crd, crd, crd_start, crd_freed, crd_freed_start;
	u32 cur_cnt = poll_count;

	crd_freed = crd_freed_start = REG_RD(bp, regs->crd_freed);
	crd = crd_start = REG_RD(bp, regs->crd);
	init_crd = REG_RD(bp, regs->init_crd);

	DP(BNX2X_MSG_SP, "INIT CREDIT[%d] : %x\n", regs->pN, init_crd);
	DP(BNX2X_MSG_SP, "CREDIT[%d]      : s:%x\n", regs->pN, crd);
	DP(BNX2X_MSG_SP, "CREDIT_FREED[%d]: s:%x\n", regs->pN, crd_freed);

	while ((crd != init_crd) && ((u32)SUB_S32(crd_freed, crd_freed_start) <
	       (init_crd - crd_start))) {
		if (cur_cnt--) {
			udelay(FLR_WAIT_INTERVAL);
			crd = REG_RD(bp, regs->crd);
			crd_freed = REG_RD(bp, regs->crd_freed);
		} else {
			DP(BNX2X_MSG_SP, "PBF tx buffer[%d] timed out\n",
			   regs->pN);
			DP(BNX2X_MSG_SP, "CREDIT[%d]      : c:%x\n",
			   regs->pN, crd);
			DP(BNX2X_MSG_SP, "CREDIT_FREED[%d]: c:%x\n",
			   regs->pN, crd_freed);
			break;
		}
	}
	DP(BNX2X_MSG_SP, "Waited %d*%d usec for PBF tx buffer[%d]\n",
	   poll_count-cur_cnt, FLR_WAIT_INTERVAL, regs->pN);
}
int bnx2x_flr_clnup_poll_hw_counter(struct bnx2x *bp, u32 reg,
				    char *msg, u32 poll_cnt)
{
	u32 val = bnx2x_flr_clnup_reg_poll(bp, reg, 0, poll_cnt);
	if (val != 0) {
		BNX2X_ERR("%s usage count=%d\n", msg, val);
		return 1;
	}
	return 0;
}
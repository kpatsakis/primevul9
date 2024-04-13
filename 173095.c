static u32 bnx2x_flr_clnup_reg_poll(struct bnx2x *bp, u32 reg,
				    u32 expected, u32 poll_count)
{
	u32 cur_cnt = poll_count;
	u32 val;

	while ((val = REG_RD(bp, reg)) != expected && cur_cnt--)
		udelay(FLR_WAIT_INTERVAL);

	return val;
}
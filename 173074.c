static void bnx2x_zero_sp_sb(struct bnx2x *bp)
{
	int func = BP_FUNC(bp);
	struct hc_sp_status_block_data sp_sb_data;
	memset(&sp_sb_data, 0, sizeof(struct hc_sp_status_block_data));

	sp_sb_data.state = SB_DISABLED;
	sp_sb_data.p_func.vf_valid = false;

	bnx2x_wr_sp_sb_data(bp, &sp_sb_data);

	bnx2x_fill(bp, BAR_CSTRORM_INTMEM +
			CSTORM_SP_STATUS_BLOCK_OFFSET(func), 0,
			CSTORM_SP_STATUS_BLOCK_SIZE);
	bnx2x_fill(bp, BAR_CSTRORM_INTMEM +
			CSTORM_SP_SYNC_BLOCK_OFFSET(func), 0,
			CSTORM_SP_SYNC_BLOCK_SIZE);
}
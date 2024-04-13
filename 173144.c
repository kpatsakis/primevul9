static void bnx2x_wr_sp_sb_data(struct bnx2x *bp,
		struct hc_sp_status_block_data *sp_sb_data)
{
	int func = BP_FUNC(bp);
	int i;
	for (i = 0; i < sizeof(struct hc_sp_status_block_data)/sizeof(u32); i++)
		REG_WR(bp, BAR_CSTRORM_INTMEM +
			CSTORM_SP_STATUS_BLOCK_DATA_OFFSET(func) +
			i*sizeof(u32),
			*((u32 *)sp_sb_data + i));
}
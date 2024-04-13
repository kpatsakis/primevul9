static void bnx2x_wr_fp_sb_data(struct bnx2x *bp,
				int fw_sb_id,
				u32 *sb_data_p,
				u32 data_size)
{
	int index;
	for (index = 0; index < data_size; index++)
		REG_WR(bp, BAR_CSTRORM_INTMEM +
			CSTORM_STATUS_BLOCK_DATA_OFFSET(fw_sb_id) +
			sizeof(u32)*index,
			*(sb_data_p + index));
}
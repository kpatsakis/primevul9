static u8 bnx2x_fp_cl_id(struct bnx2x_fastpath *fp)
{
	if (CHIP_IS_E1x(fp->bp))
		return BP_L_ID(fp->bp) + fp->index;
	else	/* We want Client ID to be the same as IGU SB ID for 57712 */
		return bnx2x_fp_igu_sb_id(fp);
}
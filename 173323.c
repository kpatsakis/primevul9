static inline u8 bnx2x_fp_fw_sb_id(struct bnx2x_fastpath *fp)
{
	return fp->bp->base_fw_ndsb + fp->index + CNIC_SUPPORT(fp->bp);
}
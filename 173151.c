static inline u8 bnx2x_fp_igu_sb_id(struct bnx2x_fastpath *fp)
{
	return fp->bp->igu_base_sb + fp->index + CNIC_SUPPORT(fp->bp);
}
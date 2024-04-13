static inline u16 bnx2x_cid_ilt_lines(struct bnx2x *bp)
{
	if (IS_SRIOV(bp))
		return (BNX2X_FIRST_VF_CID + BNX2X_VF_CIDS)/ILT_PAGE_CIDS;
	return L2_ILT_LINES(bp);
}
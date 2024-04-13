static int bnx2x_set_qm_cid_count(struct bnx2x *bp)
{
	int cid_count = BNX2X_L2_MAX_CID(bp);

	if (IS_SRIOV(bp))
		cid_count += BNX2X_VF_CIDS;

	if (CNIC_SUPPORT(bp))
		cid_count += CNIC_CID_MAX;

	return roundup(cid_count, QM_CID_ROUND);
}
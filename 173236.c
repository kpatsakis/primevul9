static void bnx2x_get_cnic_info(struct bnx2x *bp)
{
	/*
	 * iSCSI may be dynamically disabled but reading
	 * info here we will decrease memory usage by driver
	 * if the feature is disabled for good
	 */
	bnx2x_get_iscsi_info(bp);
	bnx2x_get_fcoe_info(bp);
}
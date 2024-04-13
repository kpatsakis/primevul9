void bnx2x_get_iscsi_info(struct bnx2x *bp)
{
	u32 no_flags = NO_ISCSI_FLAG;
	int port = BP_PORT(bp);
	u32 max_iscsi_conn = FW_ENCODE_32BIT_PATTERN ^ SHMEM_RD(bp,
				drv_lic_key[port].max_iscsi_conn);

	if (!CNIC_SUPPORT(bp)) {
		bp->flags |= no_flags;
		return;
	}

	/* Get the number of maximum allowed iSCSI connections */
	bp->cnic_eth_dev.max_iscsi_conn =
		(max_iscsi_conn & BNX2X_MAX_ISCSI_INIT_CONN_MASK) >>
		BNX2X_MAX_ISCSI_INIT_CONN_SHIFT;

	BNX2X_DEV_INFO("max_iscsi_conn 0x%x\n",
		       bp->cnic_eth_dev.max_iscsi_conn);

	/*
	 * If maximum allowed number of connections is zero -
	 * disable the feature.
	 */
	if (!bp->cnic_eth_dev.max_iscsi_conn)
		bp->flags |= no_flags;
}
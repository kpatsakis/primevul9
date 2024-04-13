void bnx2x_setup_cnic_info(struct bnx2x *bp)
{
	struct cnic_eth_dev *cp = &bp->cnic_eth_dev;

	cp->ctx_tbl_offset = FUNC_ILT_BASE(BP_FUNC(bp)) +
			     bnx2x_cid_ilt_lines(bp);
	cp->starting_cid = bnx2x_cid_ilt_lines(bp) * ILT_PAGE_CIDS;
	cp->fcoe_init_cid = BNX2X_FCOE_ETH_CID(bp);
	cp->iscsi_l2_cid = BNX2X_ISCSI_ETH_CID(bp);

	DP(NETIF_MSG_IFUP, "BNX2X_1st_NON_L2_ETH_CID(bp) %x, cp->starting_cid %x, cp->fcoe_init_cid %x, cp->iscsi_l2_cid %x\n",
	   BNX2X_1st_NON_L2_ETH_CID(bp), cp->starting_cid, cp->fcoe_init_cid,
	   cp->iscsi_l2_cid);

	if (NO_ISCSI_OOO(bp))
		cp->drv_state |= CNIC_DRV_STATE_NO_ISCSI_OOO;
}
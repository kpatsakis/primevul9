static void bnx2x_init_fcoe_fp(struct bnx2x *bp)
{
	struct bnx2x_fastpath *fp = bnx2x_fcoe_fp(bp);
	unsigned long q_type = 0;

	bnx2x_fcoe(bp, rx_queue) = BNX2X_NUM_ETH_QUEUES(bp);
	bnx2x_fcoe(bp, cl_id) = bnx2x_cnic_eth_cl_id(bp,
						     BNX2X_FCOE_ETH_CL_ID_IDX);
	bnx2x_fcoe(bp, cid) = BNX2X_FCOE_ETH_CID(bp);
	bnx2x_fcoe(bp, fw_sb_id) = DEF_SB_ID;
	bnx2x_fcoe(bp, igu_sb_id) = bp->igu_dsb_id;
	bnx2x_fcoe(bp, rx_cons_sb) = BNX2X_FCOE_L2_RX_INDEX;
	bnx2x_init_txdata(bp, bnx2x_fcoe(bp, txdata_ptr[0]),
			  fp->cid, FCOE_TXQ_IDX(bp), BNX2X_FCOE_L2_TX_INDEX,
			  fp);

	DP(NETIF_MSG_IFUP, "created fcoe tx data (fp index %d)\n", fp->index);

	/* qZone id equals to FW (per path) client id */
	bnx2x_fcoe(bp, cl_qzone_id) = bnx2x_fp_qzone_id(fp);
	/* init shortcut */
	bnx2x_fcoe(bp, ustorm_rx_prods_offset) =
		bnx2x_rx_ustorm_prods_offset(fp);

	/* Configure Queue State object */
	__set_bit(BNX2X_Q_TYPE_HAS_RX, &q_type);
	__set_bit(BNX2X_Q_TYPE_HAS_TX, &q_type);

	/* No multi-CoS for FCoE L2 client */
	BUG_ON(fp->max_cos != 1);

	bnx2x_init_queue_obj(bp, &bnx2x_sp_obj(bp, fp).q_obj, fp->cl_id,
			     &fp->cid, 1, BP_FUNC(bp), bnx2x_sp(bp, q_rdata),
			     bnx2x_sp_mapping(bp, q_rdata), q_type);

	DP(NETIF_MSG_IFUP,
	   "queue[%d]: bnx2x_init_sb(%p,%p) cl_id %d fw_sb %d igu_sb %d\n",
	   fp->index, bp, fp->status_blk.e2_sb, fp->cl_id, fp->fw_sb_id,
	   fp->igu_sb_id);
}
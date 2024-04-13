static void bnx2x_init_eth_fp(struct bnx2x *bp, int fp_idx)
{
	struct bnx2x_fastpath *fp = &bp->fp[fp_idx];
	u8 cos;
	unsigned long q_type = 0;
	u32 cids[BNX2X_MULTI_TX_COS] = { 0 };
	fp->rx_queue = fp_idx;
	fp->cid = fp_idx;
	fp->cl_id = bnx2x_fp_cl_id(fp);
	fp->fw_sb_id = bnx2x_fp_fw_sb_id(fp);
	fp->igu_sb_id = bnx2x_fp_igu_sb_id(fp);
	/* qZone id equals to FW (per path) client id */
	fp->cl_qzone_id  = bnx2x_fp_qzone_id(fp);

	/* init shortcut */
	fp->ustorm_rx_prods_offset = bnx2x_rx_ustorm_prods_offset(fp);

	/* Setup SB indices */
	fp->rx_cons_sb = BNX2X_RX_SB_INDEX;

	/* Configure Queue State object */
	__set_bit(BNX2X_Q_TYPE_HAS_RX, &q_type);
	__set_bit(BNX2X_Q_TYPE_HAS_TX, &q_type);

	BUG_ON(fp->max_cos > BNX2X_MULTI_TX_COS);

	/* init tx data */
	for_each_cos_in_tx_queue(fp, cos) {
		bnx2x_init_txdata(bp, fp->txdata_ptr[cos],
				  CID_COS_TO_TX_ONLY_CID(fp->cid, cos, bp),
				  FP_COS_TO_TXQ(fp, cos, bp),
				  BNX2X_TX_SB_INDEX_BASE + cos, fp);
		cids[cos] = fp->txdata_ptr[cos]->cid;
	}

	/* nothing more for vf to do here */
	if (IS_VF(bp))
		return;

	bnx2x_init_sb(bp, fp->status_blk_mapping, BNX2X_VF_ID_INVALID, false,
		      fp->fw_sb_id, fp->igu_sb_id);
	bnx2x_update_fpsb_idx(fp);
	bnx2x_init_queue_obj(bp, &bnx2x_sp_obj(bp, fp).q_obj, fp->cl_id, cids,
			     fp->max_cos, BP_FUNC(bp), bnx2x_sp(bp, q_rdata),
			     bnx2x_sp_mapping(bp, q_rdata), q_type);

	/**
	 * Configure classification DBs: Always enable Tx switching
	 */
	bnx2x_init_vlan_mac_fp_objs(fp, BNX2X_OBJ_TYPE_RX_TX);

	DP(NETIF_MSG_IFUP,
	   "queue[%d]:  bnx2x_init_sb(%p,%p)  cl_id %d  fw_sb %d  igu_sb %d\n",
	   fp_idx, bp, fp->status_blk.e2_sb, fp->cl_id, fp->fw_sb_id,
	   fp->igu_sb_id);
}
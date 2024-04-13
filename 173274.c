static void bnx2x_pf_q_prep_init(struct bnx2x *bp,
	struct bnx2x_fastpath *fp, struct bnx2x_queue_init_params *init_params)
{
	u8 cos;
	int cxt_index, cxt_offset;

	/* FCoE Queue uses Default SB, thus has no HC capabilities */
	if (!IS_FCOE_FP(fp)) {
		__set_bit(BNX2X_Q_FLG_HC, &init_params->rx.flags);
		__set_bit(BNX2X_Q_FLG_HC, &init_params->tx.flags);

		/* If HC is supported, enable host coalescing in the transition
		 * to INIT state.
		 */
		__set_bit(BNX2X_Q_FLG_HC_EN, &init_params->rx.flags);
		__set_bit(BNX2X_Q_FLG_HC_EN, &init_params->tx.flags);

		/* HC rate */
		init_params->rx.hc_rate = bp->rx_ticks ?
			(1000000 / bp->rx_ticks) : 0;
		init_params->tx.hc_rate = bp->tx_ticks ?
			(1000000 / bp->tx_ticks) : 0;

		/* FW SB ID */
		init_params->rx.fw_sb_id = init_params->tx.fw_sb_id =
			fp->fw_sb_id;

		/*
		 * CQ index among the SB indices: FCoE clients uses the default
		 * SB, therefore it's different.
		 */
		init_params->rx.sb_cq_index = HC_INDEX_ETH_RX_CQ_CONS;
		init_params->tx.sb_cq_index = HC_INDEX_ETH_FIRST_TX_CQ_CONS;
	}

	/* set maximum number of COSs supported by this queue */
	init_params->max_cos = fp->max_cos;

	DP(NETIF_MSG_IFUP, "fp: %d setting queue params max cos to: %d\n",
	    fp->index, init_params->max_cos);

	/* set the context pointers queue object */
	for (cos = FIRST_TX_COS_INDEX; cos < init_params->max_cos; cos++) {
		cxt_index = fp->txdata_ptr[cos]->cid / ILT_PAGE_CIDS;
		cxt_offset = fp->txdata_ptr[cos]->cid - (cxt_index *
				ILT_PAGE_CIDS);
		init_params->cxts[cos] =
			&bp->context[cxt_index].vcxt[cxt_offset].eth;
	}
}
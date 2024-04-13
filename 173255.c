static int bnx2x_stop_queue(struct bnx2x *bp, int index)
{
	struct bnx2x_fastpath *fp = &bp->fp[index];
	struct bnx2x_fp_txdata *txdata;
	struct bnx2x_queue_state_params q_params = {NULL};
	int rc, tx_index;

	DP(NETIF_MSG_IFDOWN, "stopping queue %d cid %d\n", index, fp->cid);

	q_params.q_obj = &bnx2x_sp_obj(bp, fp).q_obj;
	/* We want to wait for completion in this context */
	__set_bit(RAMROD_COMP_WAIT, &q_params.ramrod_flags);

	/* close tx-only connections */
	for (tx_index = FIRST_TX_ONLY_COS_INDEX;
	     tx_index < fp->max_cos;
	     tx_index++){

		/* ascertain this is a normal queue*/
		txdata = fp->txdata_ptr[tx_index];

		DP(NETIF_MSG_IFDOWN, "stopping tx-only queue %d\n",
							txdata->txq_index);

		/* send halt terminate on tx-only connection */
		q_params.cmd = BNX2X_Q_CMD_TERMINATE;
		memset(&q_params.params.terminate, 0,
		       sizeof(q_params.params.terminate));
		q_params.params.terminate.cid_index = tx_index;

		rc = bnx2x_queue_state_change(bp, &q_params);
		if (rc)
			return rc;

		/* send halt terminate on tx-only connection */
		q_params.cmd = BNX2X_Q_CMD_CFC_DEL;
		memset(&q_params.params.cfc_del, 0,
		       sizeof(q_params.params.cfc_del));
		q_params.params.cfc_del.cid_index = tx_index;
		rc = bnx2x_queue_state_change(bp, &q_params);
		if (rc)
			return rc;
	}
	/* Stop the primary connection: */
	/* ...halt the connection */
	q_params.cmd = BNX2X_Q_CMD_HALT;
	rc = bnx2x_queue_state_change(bp, &q_params);
	if (rc)
		return rc;

	/* ...terminate the connection */
	q_params.cmd = BNX2X_Q_CMD_TERMINATE;
	memset(&q_params.params.terminate, 0,
	       sizeof(q_params.params.terminate));
	q_params.params.terminate.cid_index = FIRST_TX_COS_INDEX;
	rc = bnx2x_queue_state_change(bp, &q_params);
	if (rc)
		return rc;
	/* ...delete cfc entry */
	q_params.cmd = BNX2X_Q_CMD_CFC_DEL;
	memset(&q_params.params.cfc_del, 0,
	       sizeof(q_params.params.cfc_del));
	q_params.params.cfc_del.cid_index = FIRST_TX_COS_INDEX;
	return bnx2x_queue_state_change(bp, &q_params);
}
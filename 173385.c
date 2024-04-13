static int bnx2x_setup_tx_only(struct bnx2x *bp, struct bnx2x_fastpath *fp,
			struct bnx2x_queue_state_params *q_params,
			struct bnx2x_queue_setup_tx_only_params *tx_only_params,
			int tx_index, bool leading)
{
	memset(tx_only_params, 0, sizeof(*tx_only_params));

	/* Set the command */
	q_params->cmd = BNX2X_Q_CMD_SETUP_TX_ONLY;

	/* Set tx-only QUEUE flags: don't zero statistics */
	tx_only_params->flags = bnx2x_get_common_flags(bp, fp, false);

	/* choose the index of the cid to send the slow path on */
	tx_only_params->cid_index = tx_index;

	/* Set general TX_ONLY_SETUP parameters */
	bnx2x_pf_q_prep_general(bp, fp, &tx_only_params->gen_params, tx_index);

	/* Set Tx TX_ONLY_SETUP parameters */
	bnx2x_pf_tx_q_prep(bp, fp, &tx_only_params->txq_params, tx_index);

	DP(NETIF_MSG_IFUP,
	   "preparing to send tx-only ramrod for connection: cos %d, primary cid %d, cid %d, client id %d, sp-client id %d, flags %lx\n",
	   tx_index, q_params->q_obj->cids[FIRST_TX_COS_INDEX],
	   q_params->q_obj->cids[tx_index], q_params->q_obj->cl_id,
	   tx_only_params->gen_params.spcl_id, tx_only_params->flags);

	/* send the ramrod */
	return bnx2x_queue_state_change(bp, q_params);
}
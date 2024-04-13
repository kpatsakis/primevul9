int bnx2x_setup_queue(struct bnx2x *bp, struct bnx2x_fastpath *fp,
		       bool leading)
{
	struct bnx2x_queue_state_params q_params = {NULL};
	struct bnx2x_queue_setup_params *setup_params =
						&q_params.params.setup;
	struct bnx2x_queue_setup_tx_only_params *tx_only_params =
						&q_params.params.tx_only;
	int rc;
	u8 tx_index;

	DP(NETIF_MSG_IFUP, "setting up queue %d\n", fp->index);

	/* reset IGU state skip FCoE L2 queue */
	if (!IS_FCOE_FP(fp))
		bnx2x_ack_sb(bp, fp->igu_sb_id, USTORM_ID, 0,
			     IGU_INT_ENABLE, 0);

	q_params.q_obj = &bnx2x_sp_obj(bp, fp).q_obj;
	/* We want to wait for completion in this context */
	__set_bit(RAMROD_COMP_WAIT, &q_params.ramrod_flags);

	/* Prepare the INIT parameters */
	bnx2x_pf_q_prep_init(bp, fp, &q_params.params.init);

	/* Set the command */
	q_params.cmd = BNX2X_Q_CMD_INIT;

	/* Change the state to INIT */
	rc = bnx2x_queue_state_change(bp, &q_params);
	if (rc) {
		BNX2X_ERR("Queue(%d) INIT failed\n", fp->index);
		return rc;
	}

	DP(NETIF_MSG_IFUP, "init complete\n");

	/* Now move the Queue to the SETUP state... */
	memset(setup_params, 0, sizeof(*setup_params));

	/* Set QUEUE flags */
	setup_params->flags = bnx2x_get_q_flags(bp, fp, leading);

	/* Set general SETUP parameters */
	bnx2x_pf_q_prep_general(bp, fp, &setup_params->gen_params,
				FIRST_TX_COS_INDEX);

	bnx2x_pf_rx_q_prep(bp, fp, &setup_params->pause_params,
			    &setup_params->rxq_params);

	bnx2x_pf_tx_q_prep(bp, fp, &setup_params->txq_params,
			   FIRST_TX_COS_INDEX);

	/* Set the command */
	q_params.cmd = BNX2X_Q_CMD_SETUP;

	if (IS_FCOE_FP(fp))
		bp->fcoe_init = true;

	/* Change the state to SETUP */
	rc = bnx2x_queue_state_change(bp, &q_params);
	if (rc) {
		BNX2X_ERR("Queue(%d) SETUP failed\n", fp->index);
		return rc;
	}

	/* loop through the relevant tx-only indices */
	for (tx_index = FIRST_TX_ONLY_COS_INDEX;
	      tx_index < fp->max_cos;
	      tx_index++) {

		/* prepare and send tx-only ramrod*/
		rc = bnx2x_setup_tx_only(bp, fp, &q_params,
					  tx_only_params, tx_index, leading);
		if (rc) {
			BNX2X_ERR("Queue(%d.%d) TX_ONLY_SETUP failed\n",
				  fp->index, tx_index);
			return rc;
		}
	}

	return rc;
}
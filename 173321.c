void bnx2x_sp_event(struct bnx2x_fastpath *fp, union eth_rx_cqe *rr_cqe)
{
	struct bnx2x *bp = fp->bp;
	int cid = SW_CID(rr_cqe->ramrod_cqe.conn_and_cmd_data);
	int command = CQE_CMD(rr_cqe->ramrod_cqe.conn_and_cmd_data);
	enum bnx2x_queue_cmd drv_cmd = BNX2X_Q_CMD_MAX;
	struct bnx2x_queue_sp_obj *q_obj = &bnx2x_sp_obj(bp, fp).q_obj;

	DP(BNX2X_MSG_SP,
	   "fp %d  cid %d  got ramrod #%d  state is %x  type is %d\n",
	   fp->index, cid, command, bp->state,
	   rr_cqe->ramrod_cqe.ramrod_type);

	/* If cid is within VF range, replace the slowpath object with the
	 * one corresponding to this VF
	 */
	if (cid >= BNX2X_FIRST_VF_CID  &&
	    cid < BNX2X_FIRST_VF_CID + BNX2X_VF_CIDS)
		bnx2x_iov_set_queue_sp_obj(bp, cid, &q_obj);

	switch (command) {
	case (RAMROD_CMD_ID_ETH_CLIENT_UPDATE):
		DP(BNX2X_MSG_SP, "got UPDATE ramrod. CID %d\n", cid);
		drv_cmd = BNX2X_Q_CMD_UPDATE;
		break;

	case (RAMROD_CMD_ID_ETH_CLIENT_SETUP):
		DP(BNX2X_MSG_SP, "got MULTI[%d] setup ramrod\n", cid);
		drv_cmd = BNX2X_Q_CMD_SETUP;
		break;

	case (RAMROD_CMD_ID_ETH_TX_QUEUE_SETUP):
		DP(BNX2X_MSG_SP, "got MULTI[%d] tx-only setup ramrod\n", cid);
		drv_cmd = BNX2X_Q_CMD_SETUP_TX_ONLY;
		break;

	case (RAMROD_CMD_ID_ETH_HALT):
		DP(BNX2X_MSG_SP, "got MULTI[%d] halt ramrod\n", cid);
		drv_cmd = BNX2X_Q_CMD_HALT;
		break;

	case (RAMROD_CMD_ID_ETH_TERMINATE):
		DP(BNX2X_MSG_SP, "got MULTI[%d] terminate ramrod\n", cid);
		drv_cmd = BNX2X_Q_CMD_TERMINATE;
		break;

	case (RAMROD_CMD_ID_ETH_EMPTY):
		DP(BNX2X_MSG_SP, "got MULTI[%d] empty ramrod\n", cid);
		drv_cmd = BNX2X_Q_CMD_EMPTY;
		break;

	case (RAMROD_CMD_ID_ETH_TPA_UPDATE):
		DP(BNX2X_MSG_SP, "got tpa update ramrod CID=%d\n", cid);
		drv_cmd = BNX2X_Q_CMD_UPDATE_TPA;
		break;

	default:
		BNX2X_ERR("unexpected MC reply (%d) on fp[%d]\n",
			  command, fp->index);
		return;
	}

	if ((drv_cmd != BNX2X_Q_CMD_MAX) &&
	    q_obj->complete_cmd(bp, q_obj, drv_cmd))
		/* q_obj->complete_cmd() failure means that this was
		 * an unexpected completion.
		 *
		 * In this case we don't want to increase the bp->spq_left
		 * because apparently we haven't sent this command the first
		 * place.
		 */
#ifdef BNX2X_STOP_ON_ERROR
		bnx2x_panic();
#else
		return;
#endif

	smp_mb__before_atomic();
	atomic_inc(&bp->cq_spq_left);
	/* push the change in bp->spq_left and towards the memory */
	smp_mb__after_atomic();

	DP(BNX2X_MSG_SP, "bp->cq_spq_left %x\n", atomic_read(&bp->cq_spq_left));

	if ((drv_cmd == BNX2X_Q_CMD_UPDATE) && (IS_FCOE_FP(fp)) &&
	    (!!test_bit(BNX2X_AFEX_FCOE_Q_UPDATE_PENDING, &bp->sp_state))) {
		/* if Q update ramrod is completed for last Q in AFEX vif set
		 * flow, then ACK MCP at the end
		 *
		 * mark pending ACK to MCP bit.
		 * prevent case that both bits are cleared.
		 * At the end of load/unload driver checks that
		 * sp_state is cleared, and this order prevents
		 * races
		 */
		smp_mb__before_atomic();
		set_bit(BNX2X_AFEX_PENDING_VIFSET_MCP_ACK, &bp->sp_state);
		wmb();
		clear_bit(BNX2X_AFEX_FCOE_Q_UPDATE_PENDING, &bp->sp_state);
		smp_mb__after_atomic();

		/* schedule the sp task as mcp ack is required */
		bnx2x_schedule_sp_task(bp);
	}

	return;
}
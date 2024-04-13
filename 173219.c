static void bnx2x_eq_int(struct bnx2x *bp)
{
	u16 hw_cons, sw_cons, sw_prod;
	union event_ring_elem *elem;
	u8 echo;
	u32 cid;
	u8 opcode;
	int rc, spqe_cnt = 0;
	struct bnx2x_queue_sp_obj *q_obj;
	struct bnx2x_func_sp_obj *f_obj = &bp->func_obj;
	struct bnx2x_raw_obj *rss_raw = &bp->rss_conf_obj.raw;

	hw_cons = le16_to_cpu(*bp->eq_cons_sb);

	/* The hw_cos range is 1-255, 257 - the sw_cons range is 0-254, 256.
	 * when we get the next-page we need to adjust so the loop
	 * condition below will be met. The next element is the size of a
	 * regular element and hence incrementing by 1
	 */
	if ((hw_cons & EQ_DESC_MAX_PAGE) == EQ_DESC_MAX_PAGE)
		hw_cons++;

	/* This function may never run in parallel with itself for a
	 * specific bp, thus there is no need in "paired" read memory
	 * barrier here.
	 */
	sw_cons = bp->eq_cons;
	sw_prod = bp->eq_prod;

	DP(BNX2X_MSG_SP, "EQ:  hw_cons %u  sw_cons %u bp->eq_spq_left %x\n",
			hw_cons, sw_cons, atomic_read(&bp->eq_spq_left));

	for (; sw_cons != hw_cons;
	      sw_prod = NEXT_EQ_IDX(sw_prod), sw_cons = NEXT_EQ_IDX(sw_cons)) {

		elem = &bp->eq_ring[EQ_DESC(sw_cons)];

		rc = bnx2x_iov_eq_sp_event(bp, elem);
		if (!rc) {
			DP(BNX2X_MSG_IOV, "bnx2x_iov_eq_sp_event returned %d\n",
			   rc);
			goto next_spqe;
		}

		opcode = elem->message.opcode;

		/* handle eq element */
		switch (opcode) {
		case EVENT_RING_OPCODE_VF_PF_CHANNEL:
			bnx2x_vf_mbx_schedule(bp,
					      &elem->message.data.vf_pf_event);
			continue;

		case EVENT_RING_OPCODE_STAT_QUERY:
			DP_AND((BNX2X_MSG_SP | BNX2X_MSG_STATS),
			       "got statistics comp event %d\n",
			       bp->stats_comp++);
			/* nothing to do with stats comp */
			goto next_spqe;

		case EVENT_RING_OPCODE_CFC_DEL:
			/* handle according to cid range */
			/*
			 * we may want to verify here that the bp state is
			 * HALTING
			 */

			/* elem CID originates from FW; actually LE */
			cid = SW_CID(elem->message.data.cfc_del_event.cid);

			DP(BNX2X_MSG_SP,
			   "got delete ramrod for MULTI[%d]\n", cid);

			if (CNIC_LOADED(bp) &&
			    !bnx2x_cnic_handle_cfc_del(bp, cid, elem))
				goto next_spqe;

			q_obj = bnx2x_cid_to_q_obj(bp, cid);

			if (q_obj->complete_cmd(bp, q_obj, BNX2X_Q_CMD_CFC_DEL))
				break;

			goto next_spqe;

		case EVENT_RING_OPCODE_STOP_TRAFFIC:
			DP(BNX2X_MSG_SP | BNX2X_MSG_DCB, "got STOP TRAFFIC\n");
			bnx2x_dcbx_set_params(bp, BNX2X_DCBX_STATE_TX_PAUSED);
			if (f_obj->complete_cmd(bp, f_obj,
						BNX2X_F_CMD_TX_STOP))
				break;
			goto next_spqe;

		case EVENT_RING_OPCODE_START_TRAFFIC:
			DP(BNX2X_MSG_SP | BNX2X_MSG_DCB, "got START TRAFFIC\n");
			bnx2x_dcbx_set_params(bp, BNX2X_DCBX_STATE_TX_RELEASED);
			if (f_obj->complete_cmd(bp, f_obj,
						BNX2X_F_CMD_TX_START))
				break;
			goto next_spqe;

		case EVENT_RING_OPCODE_FUNCTION_UPDATE:
			echo = elem->message.data.function_update_event.echo;
			if (echo == SWITCH_UPDATE) {
				DP(BNX2X_MSG_SP | NETIF_MSG_IFUP,
				   "got FUNC_SWITCH_UPDATE ramrod\n");
				if (f_obj->complete_cmd(
					bp, f_obj, BNX2X_F_CMD_SWITCH_UPDATE))
					break;

			} else {
				int cmd = BNX2X_SP_RTNL_AFEX_F_UPDATE;

				DP(BNX2X_MSG_SP | BNX2X_MSG_MCP,
				   "AFEX: ramrod completed FUNCTION_UPDATE\n");
				f_obj->complete_cmd(bp, f_obj,
						    BNX2X_F_CMD_AFEX_UPDATE);

				/* We will perform the Queues update from
				 * sp_rtnl task as all Queue SP operations
				 * should run under rtnl_lock.
				 */
				bnx2x_schedule_sp_rtnl(bp, cmd, 0);
			}

			goto next_spqe;

		case EVENT_RING_OPCODE_AFEX_VIF_LISTS:
			f_obj->complete_cmd(bp, f_obj,
					    BNX2X_F_CMD_AFEX_VIFLISTS);
			bnx2x_after_afex_vif_lists(bp, elem);
			goto next_spqe;
		case EVENT_RING_OPCODE_FUNCTION_START:
			DP(BNX2X_MSG_SP | NETIF_MSG_IFUP,
			   "got FUNC_START ramrod\n");
			if (f_obj->complete_cmd(bp, f_obj, BNX2X_F_CMD_START))
				break;

			goto next_spqe;

		case EVENT_RING_OPCODE_FUNCTION_STOP:
			DP(BNX2X_MSG_SP | NETIF_MSG_IFUP,
			   "got FUNC_STOP ramrod\n");
			if (f_obj->complete_cmd(bp, f_obj, BNX2X_F_CMD_STOP))
				break;

			goto next_spqe;

		case EVENT_RING_OPCODE_SET_TIMESYNC:
			DP(BNX2X_MSG_SP | BNX2X_MSG_PTP,
			   "got set_timesync ramrod completion\n");
			if (f_obj->complete_cmd(bp, f_obj,
						BNX2X_F_CMD_SET_TIMESYNC))
				break;
			goto next_spqe;
		}

		switch (opcode | bp->state) {
		case (EVENT_RING_OPCODE_RSS_UPDATE_RULES |
		      BNX2X_STATE_OPEN):
		case (EVENT_RING_OPCODE_RSS_UPDATE_RULES |
		      BNX2X_STATE_OPENING_WAIT4_PORT):
		case (EVENT_RING_OPCODE_RSS_UPDATE_RULES |
		      BNX2X_STATE_CLOSING_WAIT4_HALT):
			DP(BNX2X_MSG_SP, "got RSS_UPDATE ramrod. CID %d\n",
			   SW_CID(elem->message.data.eth_event.echo));
			rss_raw->clear_pending(rss_raw);
			break;

		case (EVENT_RING_OPCODE_SET_MAC | BNX2X_STATE_OPEN):
		case (EVENT_RING_OPCODE_SET_MAC | BNX2X_STATE_DIAG):
		case (EVENT_RING_OPCODE_SET_MAC |
		      BNX2X_STATE_CLOSING_WAIT4_HALT):
		case (EVENT_RING_OPCODE_CLASSIFICATION_RULES |
		      BNX2X_STATE_OPEN):
		case (EVENT_RING_OPCODE_CLASSIFICATION_RULES |
		      BNX2X_STATE_DIAG):
		case (EVENT_RING_OPCODE_CLASSIFICATION_RULES |
		      BNX2X_STATE_CLOSING_WAIT4_HALT):
			DP(BNX2X_MSG_SP, "got (un)set vlan/mac ramrod\n");
			bnx2x_handle_classification_eqe(bp, elem);
			break;

		case (EVENT_RING_OPCODE_MULTICAST_RULES |
		      BNX2X_STATE_OPEN):
		case (EVENT_RING_OPCODE_MULTICAST_RULES |
		      BNX2X_STATE_DIAG):
		case (EVENT_RING_OPCODE_MULTICAST_RULES |
		      BNX2X_STATE_CLOSING_WAIT4_HALT):
			DP(BNX2X_MSG_SP, "got mcast ramrod\n");
			bnx2x_handle_mcast_eqe(bp);
			break;

		case (EVENT_RING_OPCODE_FILTERS_RULES |
		      BNX2X_STATE_OPEN):
		case (EVENT_RING_OPCODE_FILTERS_RULES |
		      BNX2X_STATE_DIAG):
		case (EVENT_RING_OPCODE_FILTERS_RULES |
		      BNX2X_STATE_CLOSING_WAIT4_HALT):
			DP(BNX2X_MSG_SP, "got rx_mode ramrod\n");
			bnx2x_handle_rx_mode_eqe(bp);
			break;
		default:
			/* unknown event log error and continue */
			BNX2X_ERR("Unknown EQ event %d, bp->state 0x%x\n",
				  elem->message.opcode, bp->state);
		}
next_spqe:
		spqe_cnt++;
	} /* for */

	smp_mb__before_atomic();
	atomic_add(spqe_cnt, &bp->eq_spq_left);

	bp->eq_cons = sw_cons;
	bp->eq_prod = sw_prod;
	/* Make sure that above mem writes were issued towards the memory */
	smp_wmb();

	/* update producer */
	bnx2x_update_eq_prod(bp, bp->eq_prod);
}
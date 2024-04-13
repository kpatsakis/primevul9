static void bnx2x_sp_task(struct work_struct *work)
{
	struct bnx2x *bp = container_of(work, struct bnx2x, sp_task.work);

	DP(BNX2X_MSG_SP, "sp task invoked\n");

	/* make sure the atomic interrupt_occurred has been written */
	smp_rmb();
	if (atomic_read(&bp->interrupt_occurred)) {

		/* what work needs to be performed? */
		u16 status = bnx2x_update_dsb_idx(bp);

		DP(BNX2X_MSG_SP, "status %x\n", status);
		DP(BNX2X_MSG_SP, "setting interrupt_occurred to 0\n");
		atomic_set(&bp->interrupt_occurred, 0);

		/* HW attentions */
		if (status & BNX2X_DEF_SB_ATT_IDX) {
			bnx2x_attn_int(bp);
			status &= ~BNX2X_DEF_SB_ATT_IDX;
		}

		/* SP events: STAT_QUERY and others */
		if (status & BNX2X_DEF_SB_IDX) {
			struct bnx2x_fastpath *fp = bnx2x_fcoe_fp(bp);

			if (FCOE_INIT(bp) &&
			    (bnx2x_has_rx_work(fp) || bnx2x_has_tx_work(fp))) {
				/* Prevent local bottom-halves from running as
				 * we are going to change the local NAPI list.
				 */
				local_bh_disable();
				napi_schedule(&bnx2x_fcoe(bp, napi));
				local_bh_enable();
			}

			/* Handle EQ completions */
			bnx2x_eq_int(bp);
			bnx2x_ack_sb(bp, bp->igu_dsb_id, USTORM_ID,
				     le16_to_cpu(bp->def_idx), IGU_INT_NOP, 1);

			status &= ~BNX2X_DEF_SB_IDX;
		}

		/* if status is non zero then perhaps something went wrong */
		if (unlikely(status))
			DP(BNX2X_MSG_SP,
			   "got an unknown interrupt! (status 0x%x)\n", status);

		/* ack status block only if something was actually handled */
		bnx2x_ack_sb(bp, bp->igu_dsb_id, ATTENTION_ID,
			     le16_to_cpu(bp->def_att_idx), IGU_INT_ENABLE, 1);
	}

	/* afex - poll to check if VIFSET_ACK should be sent to MFW */
	if (test_and_clear_bit(BNX2X_AFEX_PENDING_VIFSET_MCP_ACK,
			       &bp->sp_state)) {
		bnx2x_link_report(bp);
		bnx2x_fw_command(bp, DRV_MSG_CODE_AFEX_VIFSET_ACK, 0);
	}
}
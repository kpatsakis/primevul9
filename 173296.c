static void bnx2x_after_function_update(struct bnx2x *bp)
{
	int q, rc;
	struct bnx2x_fastpath *fp;
	struct bnx2x_queue_state_params queue_params = {NULL};
	struct bnx2x_queue_update_params *q_update_params =
		&queue_params.params.update;

	/* Send Q update command with afex vlan removal values for all Qs */
	queue_params.cmd = BNX2X_Q_CMD_UPDATE;

	/* set silent vlan removal values according to vlan mode */
	__set_bit(BNX2X_Q_UPDATE_SILENT_VLAN_REM_CHNG,
		  &q_update_params->update_flags);
	__set_bit(BNX2X_Q_UPDATE_SILENT_VLAN_REM,
		  &q_update_params->update_flags);
	__set_bit(RAMROD_COMP_WAIT, &queue_params.ramrod_flags);

	/* in access mode mark mask and value are 0 to strip all vlans */
	if (bp->afex_vlan_mode == FUNC_MF_CFG_AFEX_VLAN_ACCESS_MODE) {
		q_update_params->silent_removal_value = 0;
		q_update_params->silent_removal_mask = 0;
	} else {
		q_update_params->silent_removal_value =
			(bp->afex_def_vlan_tag & VLAN_VID_MASK);
		q_update_params->silent_removal_mask = VLAN_VID_MASK;
	}

	for_each_eth_queue(bp, q) {
		/* Set the appropriate Queue object */
		fp = &bp->fp[q];
		queue_params.q_obj = &bnx2x_sp_obj(bp, fp).q_obj;

		/* send the ramrod */
		rc = bnx2x_queue_state_change(bp, &queue_params);
		if (rc < 0)
			BNX2X_ERR("Failed to config silent vlan rem for Q %d\n",
				  q);
	}

	if (!NO_FCOE(bp) && CNIC_ENABLED(bp)) {
		fp = &bp->fp[FCOE_IDX(bp)];
		queue_params.q_obj = &bnx2x_sp_obj(bp, fp).q_obj;

		/* clear pending completion bit */
		__clear_bit(RAMROD_COMP_WAIT, &queue_params.ramrod_flags);

		/* mark latest Q bit */
		smp_mb__before_atomic();
		set_bit(BNX2X_AFEX_FCOE_Q_UPDATE_PENDING, &bp->sp_state);
		smp_mb__after_atomic();

		/* send Q update ramrod for FCoE Q */
		rc = bnx2x_queue_state_change(bp, &queue_params);
		if (rc < 0)
			BNX2X_ERR("Failed to config silent vlan rem for Q %d\n",
				  q);
	} else {
		/* If no FCoE ring - ACK MCP now */
		bnx2x_link_report(bp);
		bnx2x_fw_command(bp, DRV_MSG_CODE_AFEX_VIFSET_ACK, 0);
	}
}
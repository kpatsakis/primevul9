static void bnx2x_handle_update_svid_cmd(struct bnx2x *bp)
{
	struct bnx2x_func_switch_update_params *switch_update_params;
	struct bnx2x_func_state_params func_params;

	memset(&func_params, 0, sizeof(struct bnx2x_func_state_params));
	switch_update_params = &func_params.params.switch_update;
	func_params.f_obj = &bp->func_obj;
	func_params.cmd = BNX2X_F_CMD_SWITCH_UPDATE;

	if (IS_MF_UFP(bp) || IS_MF_BD(bp)) {
		int func = BP_ABS_FUNC(bp);
		u32 val;

		/* Re-learn the S-tag from shmem */
		val = MF_CFG_RD(bp, func_mf_config[func].e1hov_tag) &
				FUNC_MF_CFG_E1HOV_TAG_MASK;
		if (val != FUNC_MF_CFG_E1HOV_TAG_DEFAULT) {
			bp->mf_ov = val;
		} else {
			BNX2X_ERR("Got an SVID event, but no tag is configured in shmem\n");
			goto fail;
		}

		/* Configure new S-tag in LLH */
		REG_WR(bp, NIG_REG_LLH0_FUNC_VLAN_ID + BP_PORT(bp) * 8,
		       bp->mf_ov);

		/* Send Ramrod to update FW of change */
		__set_bit(BNX2X_F_UPDATE_SD_VLAN_TAG_CHNG,
			  &switch_update_params->changes);
		switch_update_params->vlan = bp->mf_ov;

		if (bnx2x_func_state_change(bp, &func_params) < 0) {
			BNX2X_ERR("Failed to configure FW of S-tag Change to %02x\n",
				  bp->mf_ov);
			goto fail;
		} else {
			DP(BNX2X_MSG_MCP, "Configured S-tag %02x\n",
			   bp->mf_ov);
		}
	} else {
		goto fail;
	}

	bnx2x_fw_command(bp, DRV_MSG_CODE_OEM_UPDATE_SVID_OK, 0);
	return;
fail:
	bnx2x_fw_command(bp, DRV_MSG_CODE_OEM_UPDATE_SVID_FAILURE, 0);
}
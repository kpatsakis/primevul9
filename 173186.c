static void bnx2x_oem_event(struct bnx2x *bp, u32 event)
{
	u32 cmd_ok, cmd_fail;

	/* sanity */
	if (event & DRV_STATUS_DCC_EVENT_MASK &&
	    event & DRV_STATUS_OEM_EVENT_MASK) {
		BNX2X_ERR("Received simultaneous events %08x\n", event);
		return;
	}

	if (event & DRV_STATUS_DCC_EVENT_MASK) {
		cmd_fail = DRV_MSG_CODE_DCC_FAILURE;
		cmd_ok = DRV_MSG_CODE_DCC_OK;
	} else /* if (event & DRV_STATUS_OEM_EVENT_MASK) */ {
		cmd_fail = DRV_MSG_CODE_OEM_FAILURE;
		cmd_ok = DRV_MSG_CODE_OEM_OK;
	}

	DP(BNX2X_MSG_MCP, "oem_event 0x%x\n", event);

	if (event & (DRV_STATUS_DCC_DISABLE_ENABLE_PF |
		     DRV_STATUS_OEM_DISABLE_ENABLE_PF)) {
		/* This is the only place besides the function initialization
		 * where the bp->flags can change so it is done without any
		 * locks
		 */
		if (bp->mf_config[BP_VN(bp)] & FUNC_MF_CFG_FUNC_DISABLED) {
			DP(BNX2X_MSG_MCP, "mf_cfg function disabled\n");
			bp->flags |= MF_FUNC_DIS;

			bnx2x_e1h_disable(bp);
		} else {
			DP(BNX2X_MSG_MCP, "mf_cfg function enabled\n");
			bp->flags &= ~MF_FUNC_DIS;

			bnx2x_e1h_enable(bp);
		}
		event &= ~(DRV_STATUS_DCC_DISABLE_ENABLE_PF |
			   DRV_STATUS_OEM_DISABLE_ENABLE_PF);
	}

	if (event & (DRV_STATUS_DCC_BANDWIDTH_ALLOCATION |
		     DRV_STATUS_OEM_BANDWIDTH_ALLOCATION)) {
		bnx2x_config_mf_bw(bp);
		event &= ~(DRV_STATUS_DCC_BANDWIDTH_ALLOCATION |
			   DRV_STATUS_OEM_BANDWIDTH_ALLOCATION);
	}

	/* Report results to MCP */
	if (event)
		bnx2x_fw_command(bp, cmd_fail, 0);
	else
		bnx2x_fw_command(bp, cmd_ok, 0);
}
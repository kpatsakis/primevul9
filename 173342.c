static int bnx2x_leader_reset(struct bnx2x *bp)
{
	int rc = 0;
	bool global = bnx2x_reset_is_global(bp);
	u32 load_code;

	/* if not going to reset MCP - load "fake" driver to reset HW while
	 * driver is owner of the HW
	 */
	if (!global && !BP_NOMCP(bp)) {
		load_code = bnx2x_fw_command(bp, DRV_MSG_CODE_LOAD_REQ,
					     DRV_MSG_CODE_LOAD_REQ_WITH_LFA);
		if (!load_code) {
			BNX2X_ERR("MCP response failure, aborting\n");
			rc = -EAGAIN;
			goto exit_leader_reset;
		}
		if ((load_code != FW_MSG_CODE_DRV_LOAD_COMMON_CHIP) &&
		    (load_code != FW_MSG_CODE_DRV_LOAD_COMMON)) {
			BNX2X_ERR("MCP unexpected resp, aborting\n");
			rc = -EAGAIN;
			goto exit_leader_reset2;
		}
		load_code = bnx2x_fw_command(bp, DRV_MSG_CODE_LOAD_DONE, 0);
		if (!load_code) {
			BNX2X_ERR("MCP response failure, aborting\n");
			rc = -EAGAIN;
			goto exit_leader_reset2;
		}
	}

	/* Try to recover after the failure */
	if (bnx2x_process_kill(bp, global)) {
		BNX2X_ERR("Something bad had happen on engine %d! Aii!\n",
			  BP_PATH(bp));
		rc = -EAGAIN;
		goto exit_leader_reset2;
	}

	/*
	 * Clear RESET_IN_PROGRES and RESET_GLOBAL bits and update the driver
	 * state.
	 */
	bnx2x_set_reset_done(bp);
	if (global)
		bnx2x_clear_reset_global(bp);

exit_leader_reset2:
	/* unload "fake driver" if it was loaded */
	if (!global && !BP_NOMCP(bp)) {
		bnx2x_fw_command(bp, DRV_MSG_CODE_UNLOAD_REQ_WOL_MCP, 0);
		bnx2x_fw_command(bp, DRV_MSG_CODE_UNLOAD_DONE, 0);
	}
exit_leader_reset:
	bp->is_leader = 0;
	bnx2x_release_leader_lock(bp);
	smp_mb();
	return rc;
}
static int bnx2x_prev_mcp_done(struct bnx2x *bp)
{
	u32 rc = bnx2x_fw_command(bp, DRV_MSG_CODE_UNLOAD_DONE,
				  DRV_MSG_CODE_UNLOAD_SKIP_LINK_RESET);
	if (!rc) {
		BNX2X_ERR("MCP response failure, aborting\n");
		return -EBUSY;
	}

	return 0;
}
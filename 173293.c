void bnx2x_send_unload_done(struct bnx2x *bp, bool keep_link)
{
	u32 reset_param = keep_link ? DRV_MSG_CODE_UNLOAD_SKIP_LINK_RESET : 0;

	/* Report UNLOAD_DONE to MCP */
	if (!BP_NOMCP(bp))
		bnx2x_fw_command(bp, DRV_MSG_CODE_UNLOAD_DONE, reset_param);
}
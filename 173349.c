static void bnx2x_handle_eee_event(struct bnx2x *bp)
{
	DP(BNX2X_MSG_MCP, "EEE - LLDP event\n");
	bnx2x_fw_command(bp, DRV_MSG_CODE_EEE_RESULTS_ACK, 0);
}
static void bnx2x_set_mf_bw(struct bnx2x *bp)
{
	bnx2x_config_mf_bw(bp);
	bnx2x_fw_command(bp, DRV_MSG_CODE_SET_MF_BW_ACK, 0);
}
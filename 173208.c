u32 bnx2x_fw_command(struct bnx2x *bp, u32 command, u32 param)
{
	int mb_idx = BP_FW_MB_IDX(bp);
	u32 seq;
	u32 rc = 0;
	u32 cnt = 1;
	u8 delay = CHIP_REV_IS_SLOW(bp) ? 100 : 10;

	mutex_lock(&bp->fw_mb_mutex);
	seq = ++bp->fw_seq;
	SHMEM_WR(bp, func_mb[mb_idx].drv_mb_param, param);
	SHMEM_WR(bp, func_mb[mb_idx].drv_mb_header, (command | seq));

	DP(BNX2X_MSG_MCP, "wrote command (%x) to FW MB param 0x%08x\n",
			(command | seq), param);

	do {
		/* let the FW do it's magic ... */
		msleep(delay);

		rc = SHMEM_RD(bp, func_mb[mb_idx].fw_mb_header);

		/* Give the FW up to 5 second (500*10ms) */
	} while ((seq != (rc & FW_MSG_SEQ_NUMBER_MASK)) && (cnt++ < 500));

	DP(BNX2X_MSG_MCP, "[after %d ms] read (%x) seq is (%x) from FW MB\n",
	   cnt*delay, rc, seq);

	/* is this a reply to our command? */
	if (seq == (rc & FW_MSG_SEQ_NUMBER_MASK))
		rc &= FW_MSG_CODE_MASK;
	else {
		/* FW BUG! */
		BNX2X_ERR("FW failed to respond!\n");
		bnx2x_fw_dump(bp);
		rc = 0;
	}
	mutex_unlock(&bp->fw_mb_mutex);

	return rc;
}
void bnx2x_drv_pulse(struct bnx2x *bp)
{
	SHMEM_WR(bp, func_mb[BP_FW_MB_IDX(bp)].drv_pulse_mb,
		 bp->fw_drv_pulse_wr_seq);
}
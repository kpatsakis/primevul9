u32 bnx2x_flr_clnup_poll_count(struct bnx2x *bp)
{
	/* adjust polling timeout */
	if (CHIP_REV_IS_EMUL(bp))
		return FLR_POLL_CNT * 2000;

	if (CHIP_REV_IS_FPGA(bp))
		return FLR_POLL_CNT * 120;

	return FLR_POLL_CNT;
}
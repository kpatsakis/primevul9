static u64 bnx2x_cyclecounter_read(const struct cyclecounter *cc)
{
	struct bnx2x *bp = container_of(cc, struct bnx2x, cyclecounter);
	int port = BP_PORT(bp);
	u32 wb_data[2];
	u64 phc_cycles;

	REG_RD_DMAE(bp, port ? NIG_REG_TIMESYNC_GEN_REG + tsgen_synctime_t1 :
		    NIG_REG_TIMESYNC_GEN_REG + tsgen_synctime_t0, wb_data, 2);
	phc_cycles = wb_data[1];
	phc_cycles = (phc_cycles << 32) + wb_data[0];

	DP(BNX2X_MSG_PTP, "PHC read cycles = %llu\n", phc_cycles);

	return phc_cycles;
}
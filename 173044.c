static void bnx2x_config_mf_bw(struct bnx2x *bp)
{
	if (bp->link_vars.link_up) {
		bnx2x_cmng_fns_init(bp, true, CMNG_FNS_MINMAX);
		bnx2x_link_sync_notify(bp);
	}
	storm_memset_cmng(bp, &bp->cmng, BP_PORT(bp));
}
void bnx2x_set_local_cmng(struct bnx2x *bp)
{
	int cmng_fns = bnx2x_get_cmng_fns_mode(bp);

	if (cmng_fns != CMNG_FNS_NONE) {
		bnx2x_cmng_fns_init(bp, false, cmng_fns);
		storm_memset_cmng(bp, &bp->cmng, BP_PORT(bp));
	} else {
		/* rate shaping and fairness are disabled */
		DP(NETIF_MSG_IFUP,
		   "single function mode without fairness\n");
	}
}
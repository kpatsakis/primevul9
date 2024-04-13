static void bnx2x_cmng_fns_init(struct bnx2x *bp, u8 read_cfg, u8 cmng_type)
{
	struct cmng_init_input input;
	memset(&input, 0, sizeof(struct cmng_init_input));

	input.port_rate = bp->link_vars.line_speed;

	if (cmng_type == CMNG_FNS_MINMAX && input.port_rate) {
		int vn;

		/* read mf conf from shmem */
		if (read_cfg)
			bnx2x_read_mf_cfg(bp);

		/* vn_weight_sum and enable fairness if not 0 */
		bnx2x_calc_vn_min(bp, &input);

		/* calculate and set min-max rate for each vn */
		if (bp->port.pmf)
			for (vn = VN_0; vn < BP_MAX_VN_NUM(bp); vn++)
				bnx2x_calc_vn_max(bp, vn, &input);

		/* always enable rate shaping and fairness */
		input.flags.cmng_enables |=
					CMNG_FLAGS_PER_PORT_RATE_SHAPING_VN;

		bnx2x_init_cmng(&input, &bp->cmng);
		return;
	}

	/* rate shaping and fairness are disabled */
	DP(NETIF_MSG_IFUP,
	   "rate shaping and fairness are disabled\n");
}
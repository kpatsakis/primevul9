static void bnx2x_calc_vn_min(struct bnx2x *bp,
				      struct cmng_init_input *input)
{
	int all_zero = 1;
	int vn;

	for (vn = VN_0; vn < BP_MAX_VN_NUM(bp); vn++) {
		u32 vn_cfg = bp->mf_config[vn];
		u32 vn_min_rate = ((vn_cfg & FUNC_MF_CFG_MIN_BW_MASK) >>
				   FUNC_MF_CFG_MIN_BW_SHIFT) * 100;

		/* Skip hidden vns */
		if (vn_cfg & FUNC_MF_CFG_FUNC_HIDE)
			vn_min_rate = 0;
		/* If min rate is zero - set it to 1 */
		else if (!vn_min_rate)
			vn_min_rate = DEF_MIN_RATE;
		else
			all_zero = 0;

		input->vnic_min_rate[vn] = vn_min_rate;
	}

	/* if ETS or all min rates are zeros - disable fairness */
	if (BNX2X_IS_ETS_ENABLED(bp)) {
		input->flags.cmng_enables &=
					~CMNG_FLAGS_PER_PORT_FAIRNESS_VN;
		DP(NETIF_MSG_IFUP, "Fairness will be disabled due to ETS\n");
	} else if (all_zero) {
		input->flags.cmng_enables &=
					~CMNG_FLAGS_PER_PORT_FAIRNESS_VN;
		DP(NETIF_MSG_IFUP,
		   "All MIN values are zeroes fairness will be disabled\n");
	} else
		input->flags.cmng_enables |=
					CMNG_FLAGS_PER_PORT_FAIRNESS_VN;
}
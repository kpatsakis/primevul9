void bnx2x_read_mf_cfg(struct bnx2x *bp)
{
	int vn, n = (CHIP_MODE_IS_4_PORT(bp) ? 2 : 1);

	if (BP_NOMCP(bp))
		return; /* what should be the default value in this case */

	/* For 2 port configuration the absolute function number formula
	 * is:
	 *      abs_func = 2 * vn + BP_PORT + BP_PATH
	 *
	 *      and there are 4 functions per port
	 *
	 * For 4 port configuration it is
	 *      abs_func = 4 * vn + 2 * BP_PORT + BP_PATH
	 *
	 *      and there are 2 functions per port
	 */
	for (vn = VN_0; vn < BP_MAX_VN_NUM(bp); vn++) {
		int /*abs*/func = n * (2 * vn + BP_PORT(bp)) + BP_PATH(bp);

		if (func >= E1H_FUNC_MAX)
			break;

		bp->mf_config[vn] =
			MF_CFG_RD(bp, func_mf_config[func].config);
	}
	if (bp->mf_config[BP_VN(bp)] & FUNC_MF_CFG_FUNC_DISABLED) {
		DP(NETIF_MSG_IFUP, "mf_cfg function disabled\n");
		bp->flags |= MF_FUNC_DIS;
	} else {
		DP(NETIF_MSG_IFUP, "mf_cfg function enabled\n");
		bp->flags &= ~MF_FUNC_DIS;
	}
}
static u8 i40e_dcb_get_num_tc(struct i40e_dcbx_config *dcbcfg)
{
	int i, tc_unused = 0;
	u8 num_tc = 0;
	u8 ret = 0;

	/* Scan the ETS Config Priority Table to find
	 * traffic class enabled for a given priority
	 * and create a bitmask of enabled TCs
	 */
	for (i = 0; i < I40E_MAX_USER_PRIORITY; i++)
		num_tc |= BIT(dcbcfg->etscfg.prioritytable[i]);

	/* Now scan the bitmask to check for
	 * contiguous TCs starting with TC0
	 */
	for (i = 0; i < I40E_MAX_TRAFFIC_CLASS; i++) {
		if (num_tc & BIT(i)) {
			if (!tc_unused) {
				ret++;
			} else {
				pr_err("Non-contiguous TC - Disabling DCB\n");
				return 1;
			}
		} else {
			tc_unused = 1;
		}
	}

	/* There is always at least TC0 */
	if (!ret)
		ret = 1;

	return ret;
}
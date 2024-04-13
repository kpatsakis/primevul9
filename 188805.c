static void i40e_vsi_set_default_tc_config(struct i40e_vsi *vsi)
{
	u16 qcount;
	int i;

	/* Only TC0 is enabled */
	vsi->tc_config.numtc = 1;
	vsi->tc_config.enabled_tc = 1;
	qcount = min_t(int, vsi->alloc_queue_pairs,
		       i40e_pf_get_max_q_per_tc(vsi->back));
	for (i = 0; i < I40E_MAX_TRAFFIC_CLASS; i++) {
		/* For the TC that is not enabled set the offset to to default
		 * queue and allocate one queue for the given TC.
		 */
		vsi->tc_config.tc_info[i].qoffset = 0;
		if (i == 0)
			vsi->tc_config.tc_info[i].qcount = qcount;
		else
			vsi->tc_config.tc_info[i].qcount = 1;
		vsi->tc_config.tc_info[i].netdev_tc = 0;
	}
}
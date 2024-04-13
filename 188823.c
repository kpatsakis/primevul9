static int i40e_vsi_control_tx(struct i40e_vsi *vsi, bool enable)
{
	struct i40e_pf *pf = vsi->back;
	int i, pf_q, ret = 0;

	pf_q = vsi->base_queue;
	for (i = 0; i < vsi->num_queue_pairs; i++, pf_q++) {
		ret = i40e_control_wait_tx_q(vsi->seid, pf,
					     pf_q,
					     false /*is xdp*/, enable);
		if (ret)
			break;

		if (!i40e_enabled_xdp_vsi(vsi))
			continue;

		ret = i40e_control_wait_tx_q(vsi->seid, pf,
					     pf_q + vsi->alloc_queue_pairs,
					     true /*is xdp*/, enable);
		if (ret)
			break;
	}
	return ret;
}
void i40e_vsi_stop_rings_no_wait(struct i40e_vsi *vsi)
{
	struct i40e_pf *pf = vsi->back;
	int i, pf_q;

	pf_q = vsi->base_queue;
	for (i = 0; i < vsi->num_queue_pairs; i++, pf_q++) {
		i40e_control_tx_q(pf, pf_q, false);
		i40e_control_rx_q(pf, pf_q, false);
	}
}
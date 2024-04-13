int i40e_queue_pair_disable(struct i40e_vsi *vsi, int queue_pair)
{
	int err;

	err = i40e_enter_busy_conf(vsi);
	if (err)
		return err;

	i40e_queue_pair_disable_irq(vsi, queue_pair);
	err = i40e_queue_pair_toggle_rings(vsi, queue_pair, false /* off */);
	i40e_queue_pair_toggle_napi(vsi, queue_pair, false /* off */);
	i40e_queue_pair_clean_rings(vsi, queue_pair);
	i40e_queue_pair_reset_stats(vsi, queue_pair);

	return err;
}
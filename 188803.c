static void i40e_vsi_reinit_locked(struct i40e_vsi *vsi)
{
	struct i40e_pf *pf = vsi->back;

	WARN_ON(in_interrupt());
	while (test_and_set_bit(__I40E_CONFIG_BUSY, pf->state))
		usleep_range(1000, 2000);
	i40e_down(vsi);

	i40e_up(vsi);
	clear_bit(__I40E_CONFIG_BUSY, pf->state);
}
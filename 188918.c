static void i40e_exit_busy_conf(struct i40e_vsi *vsi)
{
	struct i40e_pf *pf = vsi->back;

	clear_bit(__I40E_CONFIG_BUSY, pf->state);
}
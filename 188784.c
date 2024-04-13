static int i40e_vsi_configure(struct i40e_vsi *vsi)
{
	int err;

	i40e_set_vsi_rx_mode(vsi);
	i40e_restore_vlan(vsi);
	i40e_vsi_config_dcb_rings(vsi);
	err = i40e_vsi_configure_tx(vsi);
	if (!err)
		err = i40e_vsi_configure_rx(vsi);

	return err;
}
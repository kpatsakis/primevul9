void i40e_vsi_stop_rings(struct i40e_vsi *vsi)
{
	/* When port TX is suspended, don't wait */
	if (test_bit(__I40E_PORT_SUSPENDED, vsi->back->state))
		return i40e_vsi_stop_rings_no_wait(vsi);

	/* do rx first for enable and last for disable
	 * Ignore return value, we need to shutdown whatever we can
	 */
	i40e_vsi_control_tx(vsi, false);
	i40e_vsi_control_rx(vsi, false);
}
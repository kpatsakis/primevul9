void i40e_vsi_remove_pvid(struct i40e_vsi *vsi)
{
	vsi->info.pvid = 0;

	i40e_vlan_stripping_disable(vsi);
}
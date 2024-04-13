void i40e_del_filter(struct i40e_vsi *vsi, const u8 *macaddr, s16 vlan)
{
	struct i40e_mac_filter *f;

	if (!vsi || !macaddr)
		return;

	f = i40e_find_filter(vsi, macaddr, vlan);
	__i40e_del_filter(vsi, f);
}
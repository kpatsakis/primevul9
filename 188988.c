static u8 i40e_mqprio_get_enabled_tc(struct i40e_pf *pf)
{
	struct i40e_vsi *vsi = pf->vsi[pf->lan_vsi];
	u8 num_tc = vsi->mqprio_qopt.qopt.num_tc;
	u8 enabled_tc = 1, i;

	for (i = 1; i < num_tc; i++)
		enabled_tc |= BIT(i);
	return enabled_tc;
}
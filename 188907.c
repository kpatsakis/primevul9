static int i40e_get_link_speed(struct i40e_vsi *vsi)
{
	struct i40e_pf *pf = vsi->back;

	switch (pf->hw.phy.link_info.link_speed) {
	case I40E_LINK_SPEED_40GB:
		return 40000;
	case I40E_LINK_SPEED_25GB:
		return 25000;
	case I40E_LINK_SPEED_20GB:
		return 20000;
	case I40E_LINK_SPEED_10GB:
		return 10000;
	case I40E_LINK_SPEED_1GB:
		return 1000;
	default:
		return -EINVAL;
	}
}
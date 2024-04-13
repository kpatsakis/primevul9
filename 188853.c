int i40e_config_rss(struct i40e_vsi *vsi, u8 *seed, u8 *lut, u16 lut_size)
{
	struct i40e_pf *pf = vsi->back;

	if (pf->hw_features & I40E_HW_RSS_AQ_CAPABLE)
		return i40e_config_rss_aq(vsi, seed, lut, lut_size);
	else
		return i40e_config_rss_reg(vsi, seed, lut, lut_size);
}
static int i40e_config_rss_aq(struct i40e_vsi *vsi, const u8 *seed,
			      u8 *lut, u16 lut_size)
{
	struct i40e_pf *pf = vsi->back;
	struct i40e_hw *hw = &pf->hw;
	int ret = 0;

	if (seed) {
		struct i40e_aqc_get_set_rss_key_data *seed_dw =
			(struct i40e_aqc_get_set_rss_key_data *)seed;
		ret = i40e_aq_set_rss_key(hw, vsi->id, seed_dw);
		if (ret) {
			dev_info(&pf->pdev->dev,
				 "Cannot set RSS key, err %s aq_err %s\n",
				 i40e_stat_str(hw, ret),
				 i40e_aq_str(hw, hw->aq.asq_last_status));
			return ret;
		}
	}
	if (lut) {
		bool pf_lut = vsi->type == I40E_VSI_MAIN ? true : false;

		ret = i40e_aq_set_rss_lut(hw, vsi->id, pf_lut, lut, lut_size);
		if (ret) {
			dev_info(&pf->pdev->dev,
				 "Cannot set RSS lut, err %s aq_err %s\n",
				 i40e_stat_str(hw, ret),
				 i40e_aq_str(hw, hw->aq.asq_last_status));
			return ret;
		}
	}
	return ret;
}
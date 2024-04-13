static int i40e_vsi_reconfig_rss(struct i40e_vsi *vsi, u16 rss_size)
{
	struct i40e_pf *pf = vsi->back;
	u8 seed[I40E_HKEY_ARRAY_SIZE];
	struct i40e_hw *hw = &pf->hw;
	int local_rss_size;
	u8 *lut;
	int ret;

	if (!vsi->rss_size)
		return -EINVAL;

	if (rss_size > vsi->rss_size)
		return -EINVAL;

	local_rss_size = min_t(int, vsi->rss_size, rss_size);
	lut = kzalloc(vsi->rss_table_size, GFP_KERNEL);
	if (!lut)
		return -ENOMEM;

	/* Ignoring user configured lut if there is one */
	i40e_fill_rss_lut(pf, lut, vsi->rss_table_size, local_rss_size);

	/* Use user configured hash key if there is one, otherwise
	 * use default.
	 */
	if (vsi->rss_hkey_user)
		memcpy(seed, vsi->rss_hkey_user, I40E_HKEY_ARRAY_SIZE);
	else
		netdev_rss_key_fill((void *)seed, I40E_HKEY_ARRAY_SIZE);

	ret = i40e_config_rss(vsi, seed, lut, vsi->rss_table_size);
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "Cannot set RSS lut, err %s aq_err %s\n",
			 i40e_stat_str(hw, ret),
			 i40e_aq_str(hw, hw->aq.asq_last_status));
		kfree(lut);
		return ret;
	}
	kfree(lut);

	/* Do the update w.r.t. storing rss_size */
	if (!vsi->orig_rss_size)
		vsi->orig_rss_size = vsi->rss_size;
	vsi->current_rss_size = local_rss_size;

	return ret;
}
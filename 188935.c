static int i40e_vsi_config_rss(struct i40e_vsi *vsi)
{
	struct i40e_pf *pf = vsi->back;
	u8 seed[I40E_HKEY_ARRAY_SIZE];
	u8 *lut;
	int ret;

	if (!(pf->hw_features & I40E_HW_RSS_AQ_CAPABLE))
		return 0;
	if (!vsi->rss_size)
		vsi->rss_size = min_t(int, pf->alloc_rss_size,
				      vsi->num_queue_pairs);
	if (!vsi->rss_size)
		return -EINVAL;
	lut = kzalloc(vsi->rss_table_size, GFP_KERNEL);
	if (!lut)
		return -ENOMEM;

	/* Use the user configured hash keys and lookup table if there is one,
	 * otherwise use default
	 */
	if (vsi->rss_lut_user)
		memcpy(lut, vsi->rss_lut_user, vsi->rss_table_size);
	else
		i40e_fill_rss_lut(pf, lut, vsi->rss_table_size, vsi->rss_size);
	if (vsi->rss_hkey_user)
		memcpy(seed, vsi->rss_hkey_user, I40E_HKEY_ARRAY_SIZE);
	else
		netdev_rss_key_fill((void *)seed, I40E_HKEY_ARRAY_SIZE);
	ret = i40e_config_rss_aq(vsi, seed, lut, vsi->rss_table_size);
	kfree(lut);
	return ret;
}
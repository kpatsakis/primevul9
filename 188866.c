static int i40e_pf_config_rss(struct i40e_pf *pf)
{
	struct i40e_vsi *vsi = pf->vsi[pf->lan_vsi];
	u8 seed[I40E_HKEY_ARRAY_SIZE];
	u8 *lut;
	struct i40e_hw *hw = &pf->hw;
	u32 reg_val;
	u64 hena;
	int ret;

	/* By default we enable TCP/UDP with IPv4/IPv6 ptypes */
	hena = (u64)i40e_read_rx_ctl(hw, I40E_PFQF_HENA(0)) |
		((u64)i40e_read_rx_ctl(hw, I40E_PFQF_HENA(1)) << 32);
	hena |= i40e_pf_get_default_rss_hena(pf);

	i40e_write_rx_ctl(hw, I40E_PFQF_HENA(0), (u32)hena);
	i40e_write_rx_ctl(hw, I40E_PFQF_HENA(1), (u32)(hena >> 32));

	/* Determine the RSS table size based on the hardware capabilities */
	reg_val = i40e_read_rx_ctl(hw, I40E_PFQF_CTL_0);
	reg_val = (pf->rss_table_size == 512) ?
			(reg_val | I40E_PFQF_CTL_0_HASHLUTSIZE_512) :
			(reg_val & ~I40E_PFQF_CTL_0_HASHLUTSIZE_512);
	i40e_write_rx_ctl(hw, I40E_PFQF_CTL_0, reg_val);

	/* Determine the RSS size of the VSI */
	if (!vsi->rss_size) {
		u16 qcount;
		/* If the firmware does something weird during VSI init, we
		 * could end up with zero TCs. Check for that to avoid
		 * divide-by-zero. It probably won't pass traffic, but it also
		 * won't panic.
		 */
		qcount = vsi->num_queue_pairs /
			 (vsi->tc_config.numtc ? vsi->tc_config.numtc : 1);
		vsi->rss_size = min_t(int, pf->alloc_rss_size, qcount);
	}
	if (!vsi->rss_size)
		return -EINVAL;

	lut = kzalloc(vsi->rss_table_size, GFP_KERNEL);
	if (!lut)
		return -ENOMEM;

	/* Use user configured lut if there is one, otherwise use default */
	if (vsi->rss_lut_user)
		memcpy(lut, vsi->rss_lut_user, vsi->rss_table_size);
	else
		i40e_fill_rss_lut(pf, lut, vsi->rss_table_size, vsi->rss_size);

	/* Use user configured hash key if there is one, otherwise
	 * use default.
	 */
	if (vsi->rss_hkey_user)
		memcpy(seed, vsi->rss_hkey_user, I40E_HKEY_ARRAY_SIZE);
	else
		netdev_rss_key_fill((void *)seed, I40E_HKEY_ARRAY_SIZE);
	ret = i40e_config_rss(vsi, seed, lut, vsi->rss_table_size);
	kfree(lut);

	return ret;
}
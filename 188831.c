static int i40e_get_rss_reg(struct i40e_vsi *vsi, u8 *seed,
			    u8 *lut, u16 lut_size)
{
	struct i40e_pf *pf = vsi->back;
	struct i40e_hw *hw = &pf->hw;
	u16 i;

	if (seed) {
		u32 *seed_dw = (u32 *)seed;

		for (i = 0; i <= I40E_PFQF_HKEY_MAX_INDEX; i++)
			seed_dw[i] = i40e_read_rx_ctl(hw, I40E_PFQF_HKEY(i));
	}
	if (lut) {
		u32 *lut_dw = (u32 *)lut;

		if (lut_size != I40E_HLUT_ARRAY_SIZE)
			return -EINVAL;
		for (i = 0; i <= I40E_PFQF_HLUT_MAX_INDEX; i++)
			lut_dw[i] = rd32(hw, I40E_PFQF_HLUT(i));
	}

	return 0;
}
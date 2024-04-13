static int i40e_config_rss_reg(struct i40e_vsi *vsi, const u8 *seed,
			       const u8 *lut, u16 lut_size)
{
	struct i40e_pf *pf = vsi->back;
	struct i40e_hw *hw = &pf->hw;
	u16 vf_id = vsi->vf_id;
	u8 i;

	/* Fill out hash function seed */
	if (seed) {
		u32 *seed_dw = (u32 *)seed;

		if (vsi->type == I40E_VSI_MAIN) {
			for (i = 0; i <= I40E_PFQF_HKEY_MAX_INDEX; i++)
				wr32(hw, I40E_PFQF_HKEY(i), seed_dw[i]);
		} else if (vsi->type == I40E_VSI_SRIOV) {
			for (i = 0; i <= I40E_VFQF_HKEY1_MAX_INDEX; i++)
				wr32(hw, I40E_VFQF_HKEY1(i, vf_id), seed_dw[i]);
		} else {
			dev_err(&pf->pdev->dev, "Cannot set RSS seed - invalid VSI type\n");
		}
	}

	if (lut) {
		u32 *lut_dw = (u32 *)lut;

		if (vsi->type == I40E_VSI_MAIN) {
			if (lut_size != I40E_HLUT_ARRAY_SIZE)
				return -EINVAL;
			for (i = 0; i <= I40E_PFQF_HLUT_MAX_INDEX; i++)
				wr32(hw, I40E_PFQF_HLUT(i), lut_dw[i]);
		} else if (vsi->type == I40E_VSI_SRIOV) {
			if (lut_size != I40E_VF_HLUT_ARRAY_SIZE)
				return -EINVAL;
			for (i = 0; i <= I40E_VFQF_HLUT_MAX_INDEX; i++)
				wr32(hw, I40E_VFQF_HLUT1(i, vf_id), lut_dw[i]);
		} else {
			dev_err(&pf->pdev->dev, "Cannot set RSS LUT - invalid VSI type\n");
		}
	}
	i40e_flush(hw);

	return 0;
}
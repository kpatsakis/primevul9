static void i40e_clear_rss_lut(struct i40e_vsi *vsi)
{
	struct i40e_pf *pf = vsi->back;
	struct i40e_hw *hw = &pf->hw;
	u16 vf_id = vsi->vf_id;
	u8 i;

	if (vsi->type == I40E_VSI_MAIN) {
		for (i = 0; i <= I40E_PFQF_HLUT_MAX_INDEX; i++)
			wr32(hw, I40E_PFQF_HLUT(i), 0);
	} else if (vsi->type == I40E_VSI_SRIOV) {
		for (i = 0; i <= I40E_VFQF_HLUT_MAX_INDEX; i++)
			i40e_write_rx_ctl(hw, I40E_VFQF_HLUT1(i, vf_id), 0);
	} else {
		dev_err(&pf->pdev->dev, "Cannot set RSS LUT - invalid VSI type\n");
	}
}
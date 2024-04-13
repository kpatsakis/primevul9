static int i40e_reset(struct i40e_pf *pf)
{
	struct i40e_hw *hw = &pf->hw;
	i40e_status ret;

	ret = i40e_pf_reset(hw);
	if (ret) {
		dev_info(&pf->pdev->dev, "PF reset failed, %d\n", ret);
		set_bit(__I40E_RESET_FAILED, pf->state);
		clear_bit(__I40E_RESET_RECOVERY_PENDING, pf->state);
	} else {
		pf->pfr_count++;
	}
	return ret;
}
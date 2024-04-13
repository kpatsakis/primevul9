static void i40e_reenable_fdir_sb(struct i40e_pf *pf)
{
	if (test_and_clear_bit(__I40E_FD_SB_AUTO_DISABLED, pf->state))
		if ((pf->flags & I40E_FLAG_FD_SB_ENABLED) &&
		    (I40E_DEBUG_FD & pf->hw.debug_mask))
			dev_info(&pf->pdev->dev, "FD Sideband/ntuple is being enabled since we have space in the table now\n");
}
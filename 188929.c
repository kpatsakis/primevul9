bool i40e_set_ntuple(struct i40e_pf *pf, netdev_features_t features)
{
	bool need_reset = false;

	/* Check if Flow Director n-tuple support was enabled or disabled.  If
	 * the state changed, we need to reset.
	 */
	if (features & NETIF_F_NTUPLE) {
		/* Enable filters and mark for reset */
		if (!(pf->flags & I40E_FLAG_FD_SB_ENABLED))
			need_reset = true;
		/* enable FD_SB only if there is MSI-X vector and no cloud
		 * filters exist
		 */
		if (pf->num_fdsb_msix > 0 && !pf->num_cloud_filters) {
			pf->flags |= I40E_FLAG_FD_SB_ENABLED;
			pf->flags &= ~I40E_FLAG_FD_SB_INACTIVE;
		}
	} else {
		/* turn off filters, mark for reset and clear SW filter list */
		if (pf->flags & I40E_FLAG_FD_SB_ENABLED) {
			need_reset = true;
			i40e_fdir_filter_exit(pf);
		}
		pf->flags &= ~I40E_FLAG_FD_SB_ENABLED;
		clear_bit(__I40E_FD_SB_AUTO_DISABLED, pf->state);
		pf->flags |= I40E_FLAG_FD_SB_INACTIVE;

		/* reset fd counters */
		pf->fd_add_err = 0;
		pf->fd_atr_cnt = 0;
		/* if ATR was auto disabled it can be re-enabled. */
		if (test_and_clear_bit(__I40E_FD_ATR_AUTO_DISABLED, pf->state))
			if ((pf->flags & I40E_FLAG_FD_ATR_ENABLED) &&
			    (I40E_DEBUG_FD & pf->hw.debug_mask))
				dev_info(&pf->pdev->dev, "ATR re-enabled.\n");
	}
	return need_reset;
}
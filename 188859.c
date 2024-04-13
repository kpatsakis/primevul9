static void i40e_prep_for_reset(struct i40e_pf *pf, bool lock_acquired)
{
	struct i40e_hw *hw = &pf->hw;
	i40e_status ret = 0;
	u32 v;

	clear_bit(__I40E_RESET_INTR_RECEIVED, pf->state);
	if (test_and_set_bit(__I40E_RESET_RECOVERY_PENDING, pf->state))
		return;
	if (i40e_check_asq_alive(&pf->hw))
		i40e_vc_notify_reset(pf);

	dev_dbg(&pf->pdev->dev, "Tearing down internal switch for reset\n");

	/* quiesce the VSIs and their queues that are not already DOWN */
	/* pf_quiesce_all_vsi modifies netdev structures -rtnl_lock needed */
	if (!lock_acquired)
		rtnl_lock();
	i40e_pf_quiesce_all_vsi(pf);
	if (!lock_acquired)
		rtnl_unlock();

	for (v = 0; v < pf->num_alloc_vsi; v++) {
		if (pf->vsi[v])
			pf->vsi[v]->seid = 0;
	}

	i40e_shutdown_adminq(&pf->hw);

	/* call shutdown HMC */
	if (hw->hmc.hmc_obj) {
		ret = i40e_shutdown_lan_hmc(hw);
		if (ret)
			dev_warn(&pf->pdev->dev,
				 "shutdown_lan_hmc failed: %d\n", ret);
	}

	/* Save the current PTP time so that we can restore the time after the
	 * reset completes.
	 */
	i40e_ptp_save_hw_time(pf);
}
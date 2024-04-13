static int __maybe_unused i40e_suspend(struct device *dev)
{
	struct i40e_pf *pf = dev_get_drvdata(dev);
	struct i40e_hw *hw = &pf->hw;

	/* If we're already suspended, then there is nothing to do */
	if (test_and_set_bit(__I40E_SUSPENDED, pf->state))
		return 0;

	set_bit(__I40E_DOWN, pf->state);

	/* Ensure service task will not be running */
	del_timer_sync(&pf->service_timer);
	cancel_work_sync(&pf->service_task);

	/* Client close must be called explicitly here because the timer
	 * has been stopped.
	 */
	i40e_notify_client_of_netdev_close(pf->vsi[pf->lan_vsi], false);

	if (pf->wol_en && (pf->hw_features & I40E_HW_WOL_MC_MAGIC_PKT_WAKE))
		i40e_enable_mc_magic_wake(pf);

	/* Since we're going to destroy queues during the
	 * i40e_clear_interrupt_scheme() we should hold the RTNL lock for this
	 * whole section
	 */
	rtnl_lock();

	i40e_prep_for_reset(pf, true);

	wr32(hw, I40E_PFPM_APM, (pf->wol_en ? I40E_PFPM_APM_APME_MASK : 0));
	wr32(hw, I40E_PFPM_WUFC, (pf->wol_en ? I40E_PFPM_WUFC_MAG_MASK : 0));

	/* Clear the interrupt scheme and release our IRQs so that the system
	 * can safely hibernate even when there are a large number of CPUs.
	 * Otherwise hibernation might fail when mapping all the vectors back
	 * to CPU0.
	 */
	i40e_clear_interrupt_scheme(pf);

	rtnl_unlock();

	return 0;
}
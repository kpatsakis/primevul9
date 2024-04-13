static void i40e_shutdown(struct pci_dev *pdev)
{
	struct i40e_pf *pf = pci_get_drvdata(pdev);
	struct i40e_hw *hw = &pf->hw;

	set_bit(__I40E_SUSPENDED, pf->state);
	set_bit(__I40E_DOWN, pf->state);

	del_timer_sync(&pf->service_timer);
	cancel_work_sync(&pf->service_task);
	i40e_cloud_filter_exit(pf);
	i40e_fdir_teardown(pf);

	/* Client close must be called explicitly here because the timer
	 * has been stopped.
	 */
	i40e_notify_client_of_netdev_close(pf->vsi[pf->lan_vsi], false);

	if (pf->wol_en && (pf->hw_features & I40E_HW_WOL_MC_MAGIC_PKT_WAKE))
		i40e_enable_mc_magic_wake(pf);

	i40e_prep_for_reset(pf, false);

	wr32(hw, I40E_PFPM_APM,
	     (pf->wol_en ? I40E_PFPM_APM_APME_MASK : 0));
	wr32(hw, I40E_PFPM_WUFC,
	     (pf->wol_en ? I40E_PFPM_WUFC_MAG_MASK : 0));

	/* Free MSI/legacy interrupt 0 when in recovery mode. */
	if (test_bit(__I40E_RECOVERY_MODE, pf->state) &&
	    !(pf->flags & I40E_FLAG_MSIX_ENABLED))
		free_irq(pf->pdev->irq, pf);

	/* Since we're going to destroy queues during the
	 * i40e_clear_interrupt_scheme() we should hold the RTNL lock for this
	 * whole section
	 */
	rtnl_lock();
	i40e_clear_interrupt_scheme(pf);
	rtnl_unlock();

	if (system_state == SYSTEM_POWER_OFF) {
		pci_wake_from_d3(pdev, pf->wol_en);
		pci_set_power_state(pdev, PCI_D3hot);
	}
}
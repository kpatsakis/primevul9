static void i40e_remove(struct pci_dev *pdev)
{
	struct i40e_pf *pf = pci_get_drvdata(pdev);
	struct i40e_hw *hw = &pf->hw;
	i40e_status ret_code;
	int i;

	i40e_dbg_pf_exit(pf);

	i40e_ptp_stop(pf);

	/* Disable RSS in hw */
	i40e_write_rx_ctl(hw, I40E_PFQF_HENA(0), 0);
	i40e_write_rx_ctl(hw, I40E_PFQF_HENA(1), 0);

	/* no more scheduling of any task */
	set_bit(__I40E_SUSPENDED, pf->state);
	set_bit(__I40E_DOWN, pf->state);
	if (pf->service_timer.function)
		del_timer_sync(&pf->service_timer);
	if (pf->service_task.func)
		cancel_work_sync(&pf->service_task);

	if (test_bit(__I40E_RECOVERY_MODE, pf->state)) {
		struct i40e_vsi *vsi = pf->vsi[0];

		/* We know that we have allocated only one vsi for this PF,
		 * it was just for registering netdevice, so the interface
		 * could be visible in the 'ifconfig' output
		 */
		unregister_netdev(vsi->netdev);
		free_netdev(vsi->netdev);

		goto unmap;
	}

	/* Client close must be called explicitly here because the timer
	 * has been stopped.
	 */
	i40e_notify_client_of_netdev_close(pf->vsi[pf->lan_vsi], false);

	if (pf->flags & I40E_FLAG_SRIOV_ENABLED) {
		i40e_free_vfs(pf);
		pf->flags &= ~I40E_FLAG_SRIOV_ENABLED;
	}

	i40e_fdir_teardown(pf);

	/* If there is a switch structure or any orphans, remove them.
	 * This will leave only the PF's VSI remaining.
	 */
	for (i = 0; i < I40E_MAX_VEB; i++) {
		if (!pf->veb[i])
			continue;

		if (pf->veb[i]->uplink_seid == pf->mac_seid ||
		    pf->veb[i]->uplink_seid == 0)
			i40e_switch_branch_release(pf->veb[i]);
	}

	/* Now we can shutdown the PF's VSI, just before we kill
	 * adminq and hmc.
	 */
	if (pf->vsi[pf->lan_vsi])
		i40e_vsi_release(pf->vsi[pf->lan_vsi]);

	i40e_cloud_filter_exit(pf);

	/* remove attached clients */
	if (pf->flags & I40E_FLAG_IWARP_ENABLED) {
		ret_code = i40e_lan_del_device(pf);
		if (ret_code)
			dev_warn(&pdev->dev, "Failed to delete client device: %d\n",
				 ret_code);
	}

	/* shutdown and destroy the HMC */
	if (hw->hmc.hmc_obj) {
		ret_code = i40e_shutdown_lan_hmc(hw);
		if (ret_code)
			dev_warn(&pdev->dev,
				 "Failed to destroy the HMC resources: %d\n",
				 ret_code);
	}

unmap:
	/* Free MSI/legacy interrupt 0 when in recovery mode. */
	if (test_bit(__I40E_RECOVERY_MODE, pf->state) &&
	    !(pf->flags & I40E_FLAG_MSIX_ENABLED))
		free_irq(pf->pdev->irq, pf);

	/* shutdown the adminq */
	i40e_shutdown_adminq(hw);

	/* destroy the locks only once, here */
	mutex_destroy(&hw->aq.arq_mutex);
	mutex_destroy(&hw->aq.asq_mutex);

	/* Clear all dynamic memory lists of rings, q_vectors, and VSIs */
	rtnl_lock();
	i40e_clear_interrupt_scheme(pf);
	for (i = 0; i < pf->num_alloc_vsi; i++) {
		if (pf->vsi[i]) {
			if (!test_bit(__I40E_RECOVERY_MODE, pf->state))
				i40e_vsi_clear_rings(pf->vsi[i]);
			i40e_vsi_clear(pf->vsi[i]);
			pf->vsi[i] = NULL;
		}
	}
	rtnl_unlock();

	for (i = 0; i < I40E_MAX_VEB; i++) {
		kfree(pf->veb[i]);
		pf->veb[i] = NULL;
	}

	kfree(pf->qp_pile);
	kfree(pf->vsi);

	iounmap(hw->hw_addr);
	kfree(pf);
	pci_release_mem_regions(pdev);

	pci_disable_pcie_error_reporting(pdev);
	pci_disable_device(pdev);
}
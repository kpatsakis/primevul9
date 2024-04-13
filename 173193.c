static void __bnx2x_remove(struct pci_dev *pdev,
			   struct net_device *dev,
			   struct bnx2x *bp,
			   bool remove_netdev)
{
	if (bp->ptp_clock) {
		ptp_clock_unregister(bp->ptp_clock);
		bp->ptp_clock = NULL;
	}

	/* Delete storage MAC address */
	if (!NO_FCOE(bp)) {
		rtnl_lock();
		dev_addr_del(bp->dev, bp->fip_mac, NETDEV_HW_ADDR_T_SAN);
		rtnl_unlock();
	}

#ifdef BCM_DCBNL
	/* Delete app tlvs from dcbnl */
	bnx2x_dcbnl_update_applist(bp, true);
#endif

	if (IS_PF(bp) &&
	    !BP_NOMCP(bp) &&
	    (bp->flags & BC_SUPPORTS_RMMOD_CMD))
		bnx2x_fw_command(bp, DRV_MSG_CODE_RMMOD, 0);

	/* Close the interface - either directly or implicitly */
	if (remove_netdev) {
		unregister_netdev(dev);
	} else {
		rtnl_lock();
		dev_close(dev);
		rtnl_unlock();
	}

	bnx2x_iov_remove_one(bp);

	/* Power on: we can't let PCI layer write to us while we are in D3 */
	if (IS_PF(bp)) {
		bnx2x_set_power_state(bp, PCI_D0);
		bnx2x_set_os_driver_state(bp, OS_DRIVER_STATE_NOT_LOADED);

		/* Set endianity registers to reset values in case next driver
		 * boots in different endianty environment.
		 */
		bnx2x_reset_endianity(bp);
	}

	/* Disable MSI/MSI-X */
	bnx2x_disable_msi(bp);

	/* Power off */
	if (IS_PF(bp))
		bnx2x_set_power_state(bp, PCI_D3hot);

	/* Make sure RESET task is not scheduled before continuing */
	cancel_delayed_work_sync(&bp->sp_rtnl_task);

	/* send message via vfpf channel to release the resources of this vf */
	if (IS_VF(bp))
		bnx2x_vfpf_release(bp);

	/* Assumes no further PCIe PM changes will occur */
	if (system_state == SYSTEM_POWER_OFF) {
		pci_wake_from_d3(pdev, bp->wol);
		pci_set_power_state(pdev, PCI_D3hot);
	}

	bnx2x_disable_pcie_error_reporting(bp);
	if (remove_netdev) {
		if (bp->regview)
			iounmap(bp->regview);

		/* For vfs, doorbells are part of the regview and were unmapped
		 * along with it. FW is only loaded by PF.
		 */
		if (IS_PF(bp)) {
			if (bp->doorbells)
				iounmap(bp->doorbells);

			bnx2x_release_firmware(bp);
		} else {
			bnx2x_vf_pci_dealloc(bp);
		}
		bnx2x_free_mem_bp(bp);

		free_netdev(dev);

		if (atomic_read(&pdev->enable_cnt) == 1)
			pci_release_regions(pdev);

		pci_disable_device(pdev);
	}
}
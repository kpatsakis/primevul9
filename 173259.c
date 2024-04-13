static pci_ers_result_t bnx2x_io_slot_reset(struct pci_dev *pdev)
{
	struct net_device *dev = pci_get_drvdata(pdev);
	struct bnx2x *bp = netdev_priv(dev);
	int i;

	rtnl_lock();
	BNX2X_ERR("IO slot reset initializing...\n");
	if (pci_enable_device(pdev)) {
		dev_err(&pdev->dev,
			"Cannot re-enable PCI device after reset\n");
		rtnl_unlock();
		return PCI_ERS_RESULT_DISCONNECT;
	}

	pci_set_master(pdev);
	pci_restore_state(pdev);
	pci_save_state(pdev);

	if (netif_running(dev))
		bnx2x_set_power_state(bp, PCI_D0);

	if (netif_running(dev)) {
		BNX2X_ERR("IO slot reset --> driver unload\n");

		/* MCP should have been reset; Need to wait for validity */
		if (bnx2x_init_shmem(bp)) {
			rtnl_unlock();
			return PCI_ERS_RESULT_DISCONNECT;
		}

		if (IS_PF(bp) && SHMEM2_HAS(bp, drv_capabilities_flag)) {
			u32 v;

			v = SHMEM2_RD(bp,
				      drv_capabilities_flag[BP_FW_MB_IDX(bp)]);
			SHMEM2_WR(bp, drv_capabilities_flag[BP_FW_MB_IDX(bp)],
				  v & ~DRV_FLAGS_CAPABILITIES_LOADED_L2);
		}
		bnx2x_drain_tx_queues(bp);
		bnx2x_send_unload_req(bp, UNLOAD_RECOVERY);
		bnx2x_netif_stop(bp, 1);
		bnx2x_free_irq(bp);

		/* Report UNLOAD_DONE to MCP */
		bnx2x_send_unload_done(bp, true);

		bp->sp_state = 0;
		bp->port.pmf = 0;

		bnx2x_prev_unload(bp);

		/* We should have reseted the engine, so It's fair to
		 * assume the FW will no longer write to the bnx2x driver.
		 */
		bnx2x_squeeze_objects(bp);
		bnx2x_free_skbs(bp);
		for_each_rx_queue(bp, i)
			bnx2x_free_rx_sge_range(bp, bp->fp + i, NUM_RX_SGE);
		bnx2x_free_fp_mem(bp);
		bnx2x_free_mem(bp);

		bp->state = BNX2X_STATE_CLOSED;
	}

	rtnl_unlock();

	/* If AER, perform cleanup of the PCIe registers */
	if (bp->flags & AER_ENABLED) {
		if (pci_cleanup_aer_uncorrect_error_status(pdev))
			BNX2X_ERR("pci_cleanup_aer_uncorrect_error_status failed\n");
		else
			DP(NETIF_MSG_HW, "pci_cleanup_aer_uncorrect_error_status succeeded\n");
	}

	return PCI_ERS_RESULT_RECOVERED;
}
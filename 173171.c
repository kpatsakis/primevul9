void bnx2x_chip_cleanup(struct bnx2x *bp, int unload_mode, bool keep_link)
{
	int port = BP_PORT(bp);
	int i, rc = 0;
	u8 cos;
	struct bnx2x_mcast_ramrod_params rparam = {NULL};
	u32 reset_code;

	/* Wait until tx fastpath tasks complete */
	for_each_tx_queue(bp, i) {
		struct bnx2x_fastpath *fp = &bp->fp[i];

		for_each_cos_in_tx_queue(fp, cos)
			rc = bnx2x_clean_tx_queue(bp, fp->txdata_ptr[cos]);
#ifdef BNX2X_STOP_ON_ERROR
		if (rc)
			return;
#endif
	}

	/* Give HW time to discard old tx messages */
	usleep_range(1000, 2000);

	/* Clean all ETH MACs */
	rc = bnx2x_del_all_macs(bp, &bp->sp_objs[0].mac_obj, BNX2X_ETH_MAC,
				false);
	if (rc < 0)
		BNX2X_ERR("Failed to delete all ETH macs: %d\n", rc);

	/* Clean up UC list  */
	rc = bnx2x_del_all_macs(bp, &bp->sp_objs[0].mac_obj, BNX2X_UC_LIST_MAC,
				true);
	if (rc < 0)
		BNX2X_ERR("Failed to schedule DEL commands for UC MACs list: %d\n",
			  rc);

	/* Disable LLH */
	if (!CHIP_IS_E1(bp))
		REG_WR(bp, NIG_REG_LLH0_FUNC_EN + port*8, 0);

	/* Set "drop all" (stop Rx).
	 * We need to take a netif_addr_lock() here in order to prevent
	 * a race between the completion code and this code.
	 */
	netif_addr_lock_bh(bp->dev);
	/* Schedule the rx_mode command */
	if (test_bit(BNX2X_FILTER_RX_MODE_PENDING, &bp->sp_state))
		set_bit(BNX2X_FILTER_RX_MODE_SCHED, &bp->sp_state);
	else if (bp->slowpath)
		bnx2x_set_storm_rx_mode(bp);

	/* Cleanup multicast configuration */
	rparam.mcast_obj = &bp->mcast_obj;
	rc = bnx2x_config_mcast(bp, &rparam, BNX2X_MCAST_CMD_DEL);
	if (rc < 0)
		BNX2X_ERR("Failed to send DEL multicast command: %d\n", rc);

	netif_addr_unlock_bh(bp->dev);

	bnx2x_iov_chip_cleanup(bp);

	/*
	 * Send the UNLOAD_REQUEST to the MCP. This will return if
	 * this function should perform FUNC, PORT or COMMON HW
	 * reset.
	 */
	reset_code = bnx2x_send_unload_req(bp, unload_mode);

	/*
	 * (assumption: No Attention from MCP at this stage)
	 * PMF probably in the middle of TX disable/enable transaction
	 */
	rc = bnx2x_func_wait_started(bp);
	if (rc) {
		BNX2X_ERR("bnx2x_func_wait_started failed\n");
#ifdef BNX2X_STOP_ON_ERROR
		return;
#endif
	}

	/* Close multi and leading connections
	 * Completions for ramrods are collected in a synchronous way
	 */
	for_each_eth_queue(bp, i)
		if (bnx2x_stop_queue(bp, i))
#ifdef BNX2X_STOP_ON_ERROR
			return;
#else
			goto unload_error;
#endif

	if (CNIC_LOADED(bp)) {
		for_each_cnic_queue(bp, i)
			if (bnx2x_stop_queue(bp, i))
#ifdef BNX2X_STOP_ON_ERROR
				return;
#else
				goto unload_error;
#endif
	}

	/* If SP settings didn't get completed so far - something
	 * very wrong has happen.
	 */
	if (!bnx2x_wait_sp_comp(bp, ~0x0UL))
		BNX2X_ERR("Hmmm... Common slow path ramrods got stuck!\n");

#ifndef BNX2X_STOP_ON_ERROR
unload_error:
#endif
	rc = bnx2x_func_stop(bp);
	if (rc) {
		BNX2X_ERR("Function stop failed!\n");
#ifdef BNX2X_STOP_ON_ERROR
		return;
#endif
	}

	/* stop_ptp should be after the Tx queues are drained to prevent
	 * scheduling to the cancelled PTP work queue. It should also be after
	 * function stop ramrod is sent, since as part of this ramrod FW access
	 * PTP registers.
	 */
	if (bp->flags & PTP_SUPPORTED)
		bnx2x_stop_ptp(bp);

	/* Disable HW interrupts, NAPI */
	bnx2x_netif_stop(bp, 1);
	/* Delete all NAPI objects */
	bnx2x_del_all_napi(bp);
	if (CNIC_LOADED(bp))
		bnx2x_del_all_napi_cnic(bp);

	/* Release IRQs */
	bnx2x_free_irq(bp);

	/* Reset the chip, unless PCI function is offline. If we reach this
	 * point following a PCI error handling, it means device is really
	 * in a bad state and we're about to remove it, so reset the chip
	 * is not a good idea.
	 */
	if (!pci_channel_offline(bp->pdev)) {
		rc = bnx2x_reset_hw(bp, reset_code);
		if (rc)
			BNX2X_ERR("HW_RESET failed\n");
	}

	/* Report UNLOAD_DONE to MCP */
	bnx2x_send_unload_done(bp, keep_link);
}
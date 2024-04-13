static void bnx2x_parity_recover(struct bnx2x *bp)
{
	bool global = false;
	u32 error_recovered, error_unrecovered;
	bool is_parity;

	DP(NETIF_MSG_HW, "Handling parity\n");
	while (1) {
		switch (bp->recovery_state) {
		case BNX2X_RECOVERY_INIT:
			DP(NETIF_MSG_HW, "State is BNX2X_RECOVERY_INIT\n");
			is_parity = bnx2x_chk_parity_attn(bp, &global, false);
			WARN_ON(!is_parity);

			/* Try to get a LEADER_LOCK HW lock */
			if (bnx2x_trylock_leader_lock(bp)) {
				bnx2x_set_reset_in_progress(bp);
				/*
				 * Check if there is a global attention and if
				 * there was a global attention, set the global
				 * reset bit.
				 */

				if (global)
					bnx2x_set_reset_global(bp);

				bp->is_leader = 1;
			}

			/* Stop the driver */
			/* If interface has been removed - break */
			if (bnx2x_nic_unload(bp, UNLOAD_RECOVERY, false))
				return;

			bp->recovery_state = BNX2X_RECOVERY_WAIT;

			/* Ensure "is_leader", MCP command sequence and
			 * "recovery_state" update values are seen on other
			 * CPUs.
			 */
			smp_mb();
			break;

		case BNX2X_RECOVERY_WAIT:
			DP(NETIF_MSG_HW, "State is BNX2X_RECOVERY_WAIT\n");
			if (bp->is_leader) {
				int other_engine = BP_PATH(bp) ? 0 : 1;
				bool other_load_status =
					bnx2x_get_load_status(bp, other_engine);
				bool load_status =
					bnx2x_get_load_status(bp, BP_PATH(bp));
				global = bnx2x_reset_is_global(bp);

				/*
				 * In case of a parity in a global block, let
				 * the first leader that performs a
				 * leader_reset() reset the global blocks in
				 * order to clear global attentions. Otherwise
				 * the gates will remain closed for that
				 * engine.
				 */
				if (load_status ||
				    (global && other_load_status)) {
					/* Wait until all other functions get
					 * down.
					 */
					schedule_delayed_work(&bp->sp_rtnl_task,
								HZ/10);
					return;
				} else {
					/* If all other functions got down -
					 * try to bring the chip back to
					 * normal. In any case it's an exit
					 * point for a leader.
					 */
					if (bnx2x_leader_reset(bp)) {
						bnx2x_recovery_failed(bp);
						return;
					}

					/* If we are here, means that the
					 * leader has succeeded and doesn't
					 * want to be a leader any more. Try
					 * to continue as a none-leader.
					 */
					break;
				}
			} else { /* non-leader */
				if (!bnx2x_reset_is_done(bp, BP_PATH(bp))) {
					/* Try to get a LEADER_LOCK HW lock as
					 * long as a former leader may have
					 * been unloaded by the user or
					 * released a leadership by another
					 * reason.
					 */
					if (bnx2x_trylock_leader_lock(bp)) {
						/* I'm a leader now! Restart a
						 * switch case.
						 */
						bp->is_leader = 1;
						break;
					}

					schedule_delayed_work(&bp->sp_rtnl_task,
								HZ/10);
					return;

				} else {
					/*
					 * If there was a global attention, wait
					 * for it to be cleared.
					 */
					if (bnx2x_reset_is_global(bp)) {
						schedule_delayed_work(
							&bp->sp_rtnl_task,
							HZ/10);
						return;
					}

					error_recovered =
					  bp->eth_stats.recoverable_error;
					error_unrecovered =
					  bp->eth_stats.unrecoverable_error;
					bp->recovery_state =
						BNX2X_RECOVERY_NIC_LOADING;
					if (bnx2x_nic_load(bp, LOAD_NORMAL)) {
						error_unrecovered++;
						netdev_err(bp->dev,
							   "Recovery failed. Power cycle needed\n");
						/* Disconnect this device */
						netif_device_detach(bp->dev);
						/* Shut down the power */
						bnx2x_set_power_state(
							bp, PCI_D3hot);
						smp_mb();
					} else {
						bp->recovery_state =
							BNX2X_RECOVERY_DONE;
						error_recovered++;
						smp_mb();
					}
					bp->eth_stats.recoverable_error =
						error_recovered;
					bp->eth_stats.unrecoverable_error =
						error_unrecovered;

					return;
				}
			}
		default:
			return;
		}
	}
}
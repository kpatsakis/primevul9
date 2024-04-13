static void bnx2x_sp_rtnl_task(struct work_struct *work)
{
	struct bnx2x *bp = container_of(work, struct bnx2x, sp_rtnl_task.work);

	rtnl_lock();

	if (!netif_running(bp->dev)) {
		rtnl_unlock();
		return;
	}

	if (unlikely(bp->recovery_state != BNX2X_RECOVERY_DONE)) {
#ifdef BNX2X_STOP_ON_ERROR
		BNX2X_ERR("recovery flow called but STOP_ON_ERROR defined so reset not done to allow debug dump,\n"
			  "you will need to reboot when done\n");
		goto sp_rtnl_not_reset;
#endif
		/*
		 * Clear all pending SP commands as we are going to reset the
		 * function anyway.
		 */
		bp->sp_rtnl_state = 0;
		smp_mb();

		bnx2x_parity_recover(bp);

		rtnl_unlock();
		return;
	}

	if (test_and_clear_bit(BNX2X_SP_RTNL_TX_TIMEOUT, &bp->sp_rtnl_state)) {
#ifdef BNX2X_STOP_ON_ERROR
		BNX2X_ERR("recovery flow called but STOP_ON_ERROR defined so reset not done to allow debug dump,\n"
			  "you will need to reboot when done\n");
		goto sp_rtnl_not_reset;
#endif

		/*
		 * Clear all pending SP commands as we are going to reset the
		 * function anyway.
		 */
		bp->sp_rtnl_state = 0;
		smp_mb();

		bnx2x_nic_unload(bp, UNLOAD_NORMAL, true);
		/* When ret value shows failure of allocation failure,
		 * the nic is rebooted again. If open still fails, a error
		 * message to notify the user.
		 */
		if (bnx2x_nic_load(bp, LOAD_NORMAL) == -ENOMEM) {
			bnx2x_nic_unload(bp, UNLOAD_NORMAL, true);
			if (bnx2x_nic_load(bp, LOAD_NORMAL))
				BNX2X_ERR("Open the NIC fails again!\n");
		}
		rtnl_unlock();
		return;
	}
#ifdef BNX2X_STOP_ON_ERROR
sp_rtnl_not_reset:
#endif
	if (test_and_clear_bit(BNX2X_SP_RTNL_SETUP_TC, &bp->sp_rtnl_state))
		bnx2x_setup_tc(bp->dev, bp->dcbx_port_params.ets.num_of_cos);
	if (test_and_clear_bit(BNX2X_SP_RTNL_AFEX_F_UPDATE, &bp->sp_rtnl_state))
		bnx2x_after_function_update(bp);
	/*
	 * in case of fan failure we need to reset id if the "stop on error"
	 * debug flag is set, since we trying to prevent permanent overheating
	 * damage
	 */
	if (test_and_clear_bit(BNX2X_SP_RTNL_FAN_FAILURE, &bp->sp_rtnl_state)) {
		DP(NETIF_MSG_HW, "fan failure detected. Unloading driver\n");
		netif_device_detach(bp->dev);
		bnx2x_close(bp->dev);
		rtnl_unlock();
		return;
	}

	if (test_and_clear_bit(BNX2X_SP_RTNL_VFPF_MCAST, &bp->sp_rtnl_state)) {
		DP(BNX2X_MSG_SP,
		   "sending set mcast vf pf channel message from rtnl sp-task\n");
		bnx2x_vfpf_set_mcast(bp->dev);
	}
	if (test_and_clear_bit(BNX2X_SP_RTNL_VFPF_CHANNEL_DOWN,
			       &bp->sp_rtnl_state)){
		if (netif_carrier_ok(bp->dev)) {
			bnx2x_tx_disable(bp);
			BNX2X_ERR("PF indicated channel is not servicable anymore. This means this VF device is no longer operational\n");
		}
	}

	if (test_and_clear_bit(BNX2X_SP_RTNL_RX_MODE, &bp->sp_rtnl_state)) {
		DP(BNX2X_MSG_SP, "Handling Rx Mode setting\n");
		bnx2x_set_rx_mode_inner(bp);
	}

	if (test_and_clear_bit(BNX2X_SP_RTNL_HYPERVISOR_VLAN,
			       &bp->sp_rtnl_state))
		bnx2x_pf_set_vfs_vlan(bp);

	if (test_and_clear_bit(BNX2X_SP_RTNL_TX_STOP, &bp->sp_rtnl_state)) {
		bnx2x_dcbx_stop_hw_tx(bp);
		bnx2x_dcbx_resume_hw_tx(bp);
	}

	if (test_and_clear_bit(BNX2X_SP_RTNL_GET_DRV_VERSION,
			       &bp->sp_rtnl_state))
		bnx2x_update_mng_version(bp);

	if (test_and_clear_bit(BNX2X_SP_RTNL_CHANGE_UDP_PORT,
			       &bp->sp_rtnl_state)) {
		if (bnx2x_udp_port_update(bp)) {
			/* On error, forget configuration */
			memset(bp->udp_tunnel_ports, 0,
			       sizeof(struct bnx2x_udp_tunnel) *
			       BNX2X_UDP_PORT_MAX);
		} else {
			/* Since we don't store additional port information,
			 * if no ports are configured for any feature ask for
			 * information about currently configured ports.
			 */
			if (!bp->udp_tunnel_ports[BNX2X_UDP_PORT_VXLAN].count &&
			    !bp->udp_tunnel_ports[BNX2X_UDP_PORT_GENEVE].count)
				udp_tunnel_get_rx_info(bp->dev);
		}
	}

	/* work which needs rtnl lock not-taken (as it takes the lock itself and
	 * can be called from other contexts as well)
	 */
	rtnl_unlock();

	/* enable SR-IOV if applicable */
	if (IS_SRIOV(bp) && test_and_clear_bit(BNX2X_SP_RTNL_ENABLE_SRIOV,
					       &bp->sp_rtnl_state)) {
		bnx2x_disable_sriov(bp);
		bnx2x_enable_sriov(bp);
	}
}
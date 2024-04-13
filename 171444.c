static int ql_finish_auto_neg(struct ql3_adapter *qdev)
{

	if (ql_sem_spinlock(qdev, QL_PHY_GIO_SEM_MASK,
		(QL_RESOURCE_BITS_BASE_CODE | (qdev->mac_index) *
			 2) << 7))
		return -1;

	if (!ql_auto_neg_error(qdev)) {
		if (test_bit(QL_LINK_MASTER, &qdev->flags)) {
			/* configure the MAC */
			netif_printk(qdev, link, KERN_DEBUG, qdev->ndev,
				     "Configuring link\n");
			ql_mac_cfg_soft_reset(qdev, 1);
			ql_mac_cfg_gig(qdev,
				       (ql_get_link_speed
					(qdev) ==
					SPEED_1000));
			ql_mac_cfg_full_dup(qdev,
					    ql_is_link_full_dup
					    (qdev));
			ql_mac_cfg_pause(qdev,
					 ql_is_neg_pause
					 (qdev));
			ql_mac_cfg_soft_reset(qdev, 0);

			/* enable the MAC */
			netif_printk(qdev, link, KERN_DEBUG, qdev->ndev,
				     "Enabling mac\n");
			ql_mac_enable(qdev, 1);
		}

		qdev->port_link_state = LS_UP;
		netif_start_queue(qdev->ndev);
		netif_carrier_on(qdev->ndev);
		netif_info(qdev, link, qdev->ndev,
			   "Link is up at %d Mbps, %s duplex\n",
			   ql_get_link_speed(qdev),
			   ql_is_link_full_dup(qdev) ? "full" : "half");

	} else {	/* Remote error detected */

		if (test_bit(QL_LINK_MASTER, &qdev->flags)) {
			netif_printk(qdev, link, KERN_DEBUG, qdev->ndev,
				     "Remote error detected. Calling ql_port_start()\n");
			/*
			 * ql_port_start() is shared code and needs
			 * to lock the PHY on it's own.
			 */
			ql_sem_unlock(qdev, QL_PHY_GIO_SEM_MASK);
			if (ql_port_start(qdev))	/* Restart port */
				return -1;
			return 0;
		}
	}
	ql_sem_unlock(qdev, QL_PHY_GIO_SEM_MASK);
	return 0;
}
static void temac_set_multicast_list(struct net_device *ndev)
{
	struct temac_local *lp = netdev_priv(ndev);
	u32 multi_addr_msw, multi_addr_lsw;
	int i = 0;
	unsigned long flags;
	bool promisc_mode_disabled = false;

	if (ndev->flags & (IFF_PROMISC | IFF_ALLMULTI) ||
	    (netdev_mc_count(ndev) > MULTICAST_CAM_TABLE_NUM)) {
		temac_indirect_out32(lp, XTE_AFM_OFFSET, XTE_AFM_EPPRM_MASK);
		dev_info(&ndev->dev, "Promiscuous mode enabled.\n");
		return;
	}

	spin_lock_irqsave(lp->indirect_lock, flags);

	if (!netdev_mc_empty(ndev)) {
		struct netdev_hw_addr *ha;

		netdev_for_each_mc_addr(ha, ndev) {
			if (WARN_ON(i >= MULTICAST_CAM_TABLE_NUM))
				break;
			multi_addr_msw = ((ha->addr[3] << 24) |
					  (ha->addr[2] << 16) |
					  (ha->addr[1] << 8) |
					  (ha->addr[0]));
			temac_indirect_out32_locked(lp, XTE_MAW0_OFFSET,
						    multi_addr_msw);
			multi_addr_lsw = ((ha->addr[5] << 8) |
					  (ha->addr[4]) | (i << 16));
			temac_indirect_out32_locked(lp, XTE_MAW1_OFFSET,
						    multi_addr_lsw);
			i++;
		}
	}

	/* Clear all or remaining/unused address table entries */
	while (i < MULTICAST_CAM_TABLE_NUM) {
		temac_indirect_out32_locked(lp, XTE_MAW0_OFFSET, 0);
		temac_indirect_out32_locked(lp, XTE_MAW1_OFFSET, i << 16);
		i++;
	}

	/* Enable address filter block if currently disabled */
	if (temac_indirect_in32_locked(lp, XTE_AFM_OFFSET)
	    & XTE_AFM_EPPRM_MASK) {
		temac_indirect_out32_locked(lp, XTE_AFM_OFFSET, 0);
		promisc_mode_disabled = true;
	}

	spin_unlock_irqrestore(lp->indirect_lock, flags);

	if (promisc_mode_disabled)
		dev_info(&ndev->dev, "Promiscuous mode disabled.\n");
}
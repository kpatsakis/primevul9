static void bnx2x_drv_info_ether_stat(struct bnx2x *bp)
{
	struct eth_stats_info *ether_stat =
		&bp->slowpath->drv_info_to_mcp.ether_stat;
	struct bnx2x_vlan_mac_obj *mac_obj =
		&bp->sp_objs->mac_obj;
	int i;

	strlcpy(ether_stat->version, DRV_MODULE_VERSION,
		ETH_STAT_INFO_VERSION_LEN);

	/* get DRV_INFO_ETH_STAT_NUM_MACS_REQUIRED macs, placing them in the
	 * mac_local field in ether_stat struct. The base address is offset by 2
	 * bytes to account for the field being 8 bytes but a mac address is
	 * only 6 bytes. Likewise, the stride for the get_n_elements function is
	 * 2 bytes to compensate from the 6 bytes of a mac to the 8 bytes
	 * allocated by the ether_stat struct, so the macs will land in their
	 * proper positions.
	 */
	for (i = 0; i < DRV_INFO_ETH_STAT_NUM_MACS_REQUIRED; i++)
		memset(ether_stat->mac_local + i, 0,
		       sizeof(ether_stat->mac_local[0]));
	mac_obj->get_n_elements(bp, &bp->sp_objs[0].mac_obj,
				DRV_INFO_ETH_STAT_NUM_MACS_REQUIRED,
				ether_stat->mac_local + MAC_PAD, MAC_PAD,
				ETH_ALEN);
	ether_stat->mtu_size = bp->dev->mtu;
	if (bp->dev->features & NETIF_F_RXCSUM)
		ether_stat->feature_flags |= FEATURE_ETH_CHKSUM_OFFLOAD_MASK;
	if (bp->dev->features & NETIF_F_TSO)
		ether_stat->feature_flags |= FEATURE_ETH_LSO_MASK;
	ether_stat->feature_flags |= bp->common.boot_mode;

	ether_stat->promiscuous_mode = (bp->dev->flags & IFF_PROMISC) ? 1 : 0;

	ether_stat->txq_size = bp->tx_ring_size;
	ether_stat->rxq_size = bp->rx_ring_size;

#ifdef CONFIG_BNX2X_SRIOV
	ether_stat->vf_cnt = IS_SRIOV(bp) ? bp->vfdb->sriov.nr_virtfn : 0;
#endif
}
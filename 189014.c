static void *i40e_fwd_add(struct net_device *netdev, struct net_device *vdev)
{
	struct i40e_netdev_priv *np = netdev_priv(netdev);
	u16 q_per_macvlan = 0, macvlan_cnt = 0, vectors;
	struct i40e_vsi *vsi = np->vsi;
	struct i40e_pf *pf = vsi->back;
	struct i40e_fwd_adapter *fwd;
	int avail_macvlan, ret;

	if ((pf->flags & I40E_FLAG_DCB_ENABLED)) {
		netdev_info(netdev, "Macvlans are not supported when DCB is enabled\n");
		return ERR_PTR(-EINVAL);
	}
	if ((pf->flags & I40E_FLAG_TC_MQPRIO)) {
		netdev_info(netdev, "Macvlans are not supported when HW TC offload is on\n");
		return ERR_PTR(-EINVAL);
	}
	if (pf->num_lan_msix < I40E_MIN_MACVLAN_VECTORS) {
		netdev_info(netdev, "Not enough vectors available to support macvlans\n");
		return ERR_PTR(-EINVAL);
	}

	/* The macvlan device has to be a single Q device so that the
	 * tc_to_txq field can be reused to pick the tx queue.
	 */
	if (netif_is_multiqueue(vdev))
		return ERR_PTR(-ERANGE);

	if (!vsi->macvlan_cnt) {
		/* reserve bit 0 for the pf device */
		set_bit(0, vsi->fwd_bitmask);

		/* Try to reserve as many queues as possible for macvlans. First
		 * reserve 3/4th of max vectors, then half, then quarter and
		 * calculate Qs per macvlan as you go
		 */
		vectors = pf->num_lan_msix;
		if (vectors <= I40E_MAX_MACVLANS && vectors > 64) {
			/* allocate 4 Qs per macvlan and 32 Qs to the PF*/
			q_per_macvlan = 4;
			macvlan_cnt = (vectors - 32) / 4;
		} else if (vectors <= 64 && vectors > 32) {
			/* allocate 2 Qs per macvlan and 16 Qs to the PF*/
			q_per_macvlan = 2;
			macvlan_cnt = (vectors - 16) / 2;
		} else if (vectors <= 32 && vectors > 16) {
			/* allocate 1 Q per macvlan and 16 Qs to the PF*/
			q_per_macvlan = 1;
			macvlan_cnt = vectors - 16;
		} else if (vectors <= 16 && vectors > 8) {
			/* allocate 1 Q per macvlan and 8 Qs to the PF */
			q_per_macvlan = 1;
			macvlan_cnt = vectors - 8;
		} else {
			/* allocate 1 Q per macvlan and 1 Q to the PF */
			q_per_macvlan = 1;
			macvlan_cnt = vectors - 1;
		}

		if (macvlan_cnt == 0)
			return ERR_PTR(-EBUSY);

		/* Quiesce VSI queues */
		i40e_quiesce_vsi(vsi);

		/* sets up the macvlans but does not "enable" them */
		ret = i40e_setup_macvlans(vsi, macvlan_cnt, q_per_macvlan,
					  vdev);
		if (ret)
			return ERR_PTR(ret);

		/* Unquiesce VSI */
		i40e_unquiesce_vsi(vsi);
	}
	avail_macvlan = find_first_zero_bit(vsi->fwd_bitmask,
					    vsi->macvlan_cnt);
	if (avail_macvlan >= I40E_MAX_MACVLANS)
		return ERR_PTR(-EBUSY);

	/* create the fwd struct */
	fwd = kzalloc(sizeof(*fwd), GFP_KERNEL);
	if (!fwd)
		return ERR_PTR(-ENOMEM);

	set_bit(avail_macvlan, vsi->fwd_bitmask);
	fwd->bit_no = avail_macvlan;
	netdev_set_sb_channel(vdev, avail_macvlan);
	fwd->netdev = vdev;

	if (!netif_running(netdev))
		return fwd;

	/* Set fwd ring up */
	ret = i40e_fwd_ring_up(vsi, vdev, fwd);
	if (ret) {
		/* unbind the queues and drop the subordinate channel config */
		netdev_unbind_sb_channel(netdev, vdev);
		netdev_set_sb_channel(vdev, 0);

		kfree(fwd);
		return ERR_PTR(-EINVAL);
	}

	return fwd;
}
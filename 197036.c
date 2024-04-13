static void bond_compute_features(struct bonding *bond)
{
	unsigned int dst_release_flag = IFF_XMIT_DST_RELEASE |
					IFF_XMIT_DST_RELEASE_PERM;
	netdev_features_t vlan_features = BOND_VLAN_FEATURES;
	netdev_features_t enc_features  = BOND_ENC_FEATURES;
#ifdef CONFIG_XFRM_OFFLOAD
	netdev_features_t xfrm_features  = BOND_XFRM_FEATURES;
#endif /* CONFIG_XFRM_OFFLOAD */
	netdev_features_t mpls_features  = BOND_MPLS_FEATURES;
	struct net_device *bond_dev = bond->dev;
	struct list_head *iter;
	struct slave *slave;
	unsigned short max_hard_header_len = ETH_HLEN;
	unsigned int gso_max_size = GSO_MAX_SIZE;
	u16 gso_max_segs = GSO_MAX_SEGS;

	if (!bond_has_slaves(bond))
		goto done;
	vlan_features &= NETIF_F_ALL_FOR_ALL;
	mpls_features &= NETIF_F_ALL_FOR_ALL;

	bond_for_each_slave(bond, slave, iter) {
		vlan_features = netdev_increment_features(vlan_features,
			slave->dev->vlan_features, BOND_VLAN_FEATURES);

		enc_features = netdev_increment_features(enc_features,
							 slave->dev->hw_enc_features,
							 BOND_ENC_FEATURES);

#ifdef CONFIG_XFRM_OFFLOAD
		xfrm_features = netdev_increment_features(xfrm_features,
							  slave->dev->hw_enc_features,
							  BOND_XFRM_FEATURES);
#endif /* CONFIG_XFRM_OFFLOAD */

		mpls_features = netdev_increment_features(mpls_features,
							  slave->dev->mpls_features,
							  BOND_MPLS_FEATURES);

		dst_release_flag &= slave->dev->priv_flags;
		if (slave->dev->hard_header_len > max_hard_header_len)
			max_hard_header_len = slave->dev->hard_header_len;

		gso_max_size = min(gso_max_size, slave->dev->gso_max_size);
		gso_max_segs = min(gso_max_segs, slave->dev->gso_max_segs);
	}
	bond_dev->hard_header_len = max_hard_header_len;

done:
	bond_dev->vlan_features = vlan_features;
	bond_dev->hw_enc_features = enc_features | NETIF_F_GSO_ENCAP_ALL |
				    NETIF_F_HW_VLAN_CTAG_TX |
				    NETIF_F_HW_VLAN_STAG_TX;
#ifdef CONFIG_XFRM_OFFLOAD
	bond_dev->hw_enc_features |= xfrm_features;
#endif /* CONFIG_XFRM_OFFLOAD */
	bond_dev->mpls_features = mpls_features;
	bond_dev->gso_max_segs = gso_max_segs;
	netif_set_gso_max_size(bond_dev, gso_max_size);

	bond_dev->priv_flags &= ~IFF_XMIT_DST_RELEASE;
	if ((bond_dev->priv_flags & IFF_XMIT_DST_RELEASE_PERM) &&
	    dst_release_flag == (IFF_XMIT_DST_RELEASE | IFF_XMIT_DST_RELEASE_PERM))
		bond_dev->priv_flags |= IFF_XMIT_DST_RELEASE;

	netdev_change_features(bond_dev);
}
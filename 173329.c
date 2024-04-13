static int bnx2x_vlan_rx_add_vid(struct net_device *dev, __be16 proto, u16 vid)
{
	struct bnx2x *bp = netdev_priv(dev);
	struct bnx2x_vlan_entry *vlan;

	DP(NETIF_MSG_IFUP, "Adding VLAN %d\n", vid);

	vlan = kmalloc(sizeof(*vlan), GFP_KERNEL);
	if (!vlan)
		return -ENOMEM;

	vlan->vid = vid;
	vlan->hw = false;
	list_add_tail(&vlan->link, &bp->vlan_reg);

	if (netif_running(dev))
		bnx2x_vlan_configure(bp, true);

	return 0;
}
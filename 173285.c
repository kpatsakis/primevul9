static int bnx2x_vlan_rx_kill_vid(struct net_device *dev, __be16 proto, u16 vid)
{
	struct bnx2x *bp = netdev_priv(dev);
	struct bnx2x_vlan_entry *vlan;
	bool found = false;
	int rc = 0;

	DP(NETIF_MSG_IFUP, "Removing VLAN %d\n", vid);

	list_for_each_entry(vlan, &bp->vlan_reg, link)
		if (vlan->vid == vid) {
			found = true;
			break;
		}

	if (!found) {
		BNX2X_ERR("Unable to kill VLAN %d - not found\n", vid);
		return -EINVAL;
	}

	if (netif_running(dev) && vlan->hw) {
		rc = __bnx2x_vlan_configure_vid(bp, vid, false);
		DP(NETIF_MSG_IFUP, "HW deconfigured for VLAN %d\n", vid);
		bp->vlan_cnt--;
	}

	list_del(&vlan->link);
	kfree(vlan);

	if (netif_running(dev))
		bnx2x_vlan_configure(bp, true);

	DP(NETIF_MSG_IFUP, "Removing VLAN result %d\n", rc);

	return rc;
}
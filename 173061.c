static int bnx2x_vlan_configure_vid_list(struct bnx2x *bp)
{
	struct bnx2x_vlan_entry *vlan;
	int rc = 0;

	/* Configure all non-configured entries */
	list_for_each_entry(vlan, &bp->vlan_reg, link) {
		if (vlan->hw)
			continue;

		if (bp->vlan_cnt >= bp->vlan_credit)
			return -ENOBUFS;

		rc = __bnx2x_vlan_configure_vid(bp, vlan->vid, true);
		if (rc) {
			BNX2X_ERR("Unable to config VLAN %d\n", vlan->vid);
			return rc;
		}

		DP(NETIF_MSG_IFUP, "HW configured for VLAN %d\n", vlan->vid);
		vlan->hw = true;
		bp->vlan_cnt++;
	}

	return 0;
}
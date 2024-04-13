static int bnx2x_set_uc_list(struct bnx2x *bp)
{
	int rc;
	struct net_device *dev = bp->dev;
	struct netdev_hw_addr *ha;
	struct bnx2x_vlan_mac_obj *mac_obj = &bp->sp_objs->mac_obj;
	unsigned long ramrod_flags = 0;

	/* First schedule a cleanup up of old configuration */
	rc = bnx2x_del_all_macs(bp, mac_obj, BNX2X_UC_LIST_MAC, false);
	if (rc < 0) {
		BNX2X_ERR("Failed to schedule DELETE operations: %d\n", rc);
		return rc;
	}

	netdev_for_each_uc_addr(ha, dev) {
		rc = bnx2x_set_mac_one(bp, bnx2x_uc_addr(ha), mac_obj, true,
				       BNX2X_UC_LIST_MAC, &ramrod_flags);
		if (rc == -EEXIST) {
			DP(BNX2X_MSG_SP,
			   "Failed to schedule ADD operations: %d\n", rc);
			/* do not treat adding same MAC as error */
			rc = 0;

		} else if (rc < 0) {

			BNX2X_ERR("Failed to schedule ADD operations: %d\n",
				  rc);
			return rc;
		}
	}

	/* Execute the pending commands */
	__set_bit(RAMROD_CONT, &ramrod_flags);
	return bnx2x_set_mac_one(bp, NULL, mac_obj, false /* don't care */,
				 BNX2X_UC_LIST_MAC, &ramrod_flags);
}
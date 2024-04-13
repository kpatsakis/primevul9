int bnx2x_set_mac_one(struct bnx2x *bp, u8 *mac,
		      struct bnx2x_vlan_mac_obj *obj, bool set,
		      int mac_type, unsigned long *ramrod_flags)
{
	int rc;
	struct bnx2x_vlan_mac_ramrod_params ramrod_param;

	memset(&ramrod_param, 0, sizeof(ramrod_param));

	/* Fill general parameters */
	ramrod_param.vlan_mac_obj = obj;
	ramrod_param.ramrod_flags = *ramrod_flags;

	/* Fill a user request section if needed */
	if (!test_bit(RAMROD_CONT, ramrod_flags)) {
		memcpy(ramrod_param.user_req.u.mac.mac, mac, ETH_ALEN);

		__set_bit(mac_type, &ramrod_param.user_req.vlan_mac_flags);

		/* Set the command: ADD or DEL */
		if (set)
			ramrod_param.user_req.cmd = BNX2X_VLAN_MAC_ADD;
		else
			ramrod_param.user_req.cmd = BNX2X_VLAN_MAC_DEL;
	}

	rc = bnx2x_config_vlan_mac(bp, &ramrod_param);

	if (rc == -EEXIST) {
		DP(BNX2X_MSG_SP, "Failed to schedule ADD operations: %d\n", rc);
		/* do not treat adding same MAC as error */
		rc = 0;
	} else if (rc < 0)
		BNX2X_ERR("%s MAC failed\n", (set ? "Set" : "Del"));

	return rc;
}
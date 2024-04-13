int bnx2x_set_vlan_one(struct bnx2x *bp, u16 vlan,
		       struct bnx2x_vlan_mac_obj *obj, bool set,
		       unsigned long *ramrod_flags)
{
	int rc;
	struct bnx2x_vlan_mac_ramrod_params ramrod_param;

	memset(&ramrod_param, 0, sizeof(ramrod_param));

	/* Fill general parameters */
	ramrod_param.vlan_mac_obj = obj;
	ramrod_param.ramrod_flags = *ramrod_flags;

	/* Fill a user request section if needed */
	if (!test_bit(RAMROD_CONT, ramrod_flags)) {
		ramrod_param.user_req.u.vlan.vlan = vlan;
		/* Set the command: ADD or DEL */
		if (set)
			ramrod_param.user_req.cmd = BNX2X_VLAN_MAC_ADD;
		else
			ramrod_param.user_req.cmd = BNX2X_VLAN_MAC_DEL;
	}

	rc = bnx2x_config_vlan_mac(bp, &ramrod_param);

	if (rc == -EEXIST) {
		/* Do not treat adding same vlan as error. */
		DP(BNX2X_MSG_SP, "Failed to schedule ADD operations: %d\n", rc);
		rc = 0;
	} else if (rc < 0) {
		BNX2X_ERR("%s VLAN failed\n", (set ? "Set" : "Del"));
	}

	return rc;
}
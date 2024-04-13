int bnx2x_del_all_macs(struct bnx2x *bp,
		       struct bnx2x_vlan_mac_obj *mac_obj,
		       int mac_type, bool wait_for_comp)
{
	int rc;
	unsigned long ramrod_flags = 0, vlan_mac_flags = 0;

	/* Wait for completion of requested */
	if (wait_for_comp)
		__set_bit(RAMROD_COMP_WAIT, &ramrod_flags);

	/* Set the mac type of addresses we want to clear */
	__set_bit(mac_type, &vlan_mac_flags);

	rc = mac_obj->delete_all(bp, mac_obj, &vlan_mac_flags, &ramrod_flags);
	if (rc < 0)
		BNX2X_ERR("Failed to delete MACs: %d\n", rc);

	return rc;
}
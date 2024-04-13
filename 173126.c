static void bnx2x_handle_classification_eqe(struct bnx2x *bp,
					    union event_ring_elem *elem)
{
	unsigned long ramrod_flags = 0;
	int rc = 0;
	u32 echo = le32_to_cpu(elem->message.data.eth_event.echo);
	u32 cid = echo & BNX2X_SWCID_MASK;
	struct bnx2x_vlan_mac_obj *vlan_mac_obj;

	/* Always push next commands out, don't wait here */
	__set_bit(RAMROD_CONT, &ramrod_flags);

	switch (echo >> BNX2X_SWCID_SHIFT) {
	case BNX2X_FILTER_MAC_PENDING:
		DP(BNX2X_MSG_SP, "Got SETUP_MAC completions\n");
		if (CNIC_LOADED(bp) && (cid == BNX2X_ISCSI_ETH_CID(bp)))
			vlan_mac_obj = &bp->iscsi_l2_mac_obj;
		else
			vlan_mac_obj = &bp->sp_objs[cid].mac_obj;

		break;
	case BNX2X_FILTER_VLAN_PENDING:
		DP(BNX2X_MSG_SP, "Got SETUP_VLAN completions\n");
		vlan_mac_obj = &bp->sp_objs[cid].vlan_obj;
		break;
	case BNX2X_FILTER_MCAST_PENDING:
		DP(BNX2X_MSG_SP, "Got SETUP_MCAST completions\n");
		/* This is only relevant for 57710 where multicast MACs are
		 * configured as unicast MACs using the same ramrod.
		 */
		bnx2x_handle_mcast_eqe(bp);
		return;
	default:
		BNX2X_ERR("Unsupported classification command: 0x%x\n", echo);
		return;
	}

	rc = vlan_mac_obj->complete(bp, vlan_mac_obj, elem, &ramrod_flags);

	if (rc < 0)
		BNX2X_ERR("Failed to schedule new commands: %d\n", rc);
	else if (rc > 0)
		DP(BNX2X_MSG_SP, "Scheduled next pending commands...\n");
}
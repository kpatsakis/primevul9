static bool bond_should_deliver_exact_match(struct sk_buff *skb,
					    struct slave *slave,
					    struct bonding *bond)
{
	if (bond_is_slave_inactive(slave)) {
		if (BOND_MODE(bond) == BOND_MODE_ALB &&
		    skb->pkt_type != PACKET_BROADCAST &&
		    skb->pkt_type != PACKET_MULTICAST)
			return false;
		return true;
	}
	return false;
}
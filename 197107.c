static rx_handler_result_t bond_handle_frame(struct sk_buff **pskb)
{
	struct sk_buff *skb = *pskb;
	struct slave *slave;
	struct bonding *bond;
	int (*recv_probe)(const struct sk_buff *, struct bonding *,
			  struct slave *);
	int ret = RX_HANDLER_ANOTHER;

	skb = skb_share_check(skb, GFP_ATOMIC);
	if (unlikely(!skb))
		return RX_HANDLER_CONSUMED;

	*pskb = skb;

	slave = bond_slave_get_rcu(skb->dev);
	bond = slave->bond;

	recv_probe = READ_ONCE(bond->recv_probe);
	if (recv_probe) {
		ret = recv_probe(skb, bond, slave);
		if (ret == RX_HANDLER_CONSUMED) {
			consume_skb(skb);
			return ret;
		}
	}

	/*
	 * For packets determined by bond_should_deliver_exact_match() call to
	 * be suppressed we want to make an exception for link-local packets.
	 * This is necessary for e.g. LLDP daemons to be able to monitor
	 * inactive slave links without being forced to bind to them
	 * explicitly.
	 *
	 * At the same time, packets that are passed to the bonding master
	 * (including link-local ones) can have their originating interface
	 * determined via PACKET_ORIGDEV socket option.
	 */
	if (bond_should_deliver_exact_match(skb, slave, bond)) {
		if (is_link_local_ether_addr(eth_hdr(skb)->h_dest))
			return RX_HANDLER_PASS;
		return RX_HANDLER_EXACT;
	}

	skb->dev = bond->dev;

	if (BOND_MODE(bond) == BOND_MODE_ALB &&
	    netif_is_bridge_port(bond->dev) &&
	    skb->pkt_type == PACKET_HOST) {

		if (unlikely(skb_cow_head(skb,
					  skb->data - skb_mac_header(skb)))) {
			kfree_skb(skb);
			return RX_HANDLER_CONSUMED;
		}
		bond_hw_addr_copy(eth_hdr(skb)->h_dest, bond->dev->dev_addr,
				  bond->dev->addr_len);
	}

	return ret;
}
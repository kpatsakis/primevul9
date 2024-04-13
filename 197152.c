static struct slave *bond_xmit_roundrobin_slave_get(struct bonding *bond,
						    struct sk_buff *skb)
{
	struct slave *slave;
	int slave_cnt;
	u32 slave_id;

	/* Start with the curr_active_slave that joined the bond as the
	 * default for sending IGMP traffic.  For failover purposes one
	 * needs to maintain some consistency for the interface that will
	 * send the join/membership reports.  The curr_active_slave found
	 * will send all of this type of traffic.
	 */
	if (skb->protocol == htons(ETH_P_IP)) {
		int noff = skb_network_offset(skb);
		struct iphdr *iph;

		if (unlikely(!pskb_may_pull(skb, noff + sizeof(*iph))))
			goto non_igmp;

		iph = ip_hdr(skb);
		if (iph->protocol == IPPROTO_IGMP) {
			slave = rcu_dereference(bond->curr_active_slave);
			if (slave)
				return slave;
			return bond_get_slave_by_id(bond, 0);
		}
	}

non_igmp:
	slave_cnt = READ_ONCE(bond->slave_cnt);
	if (likely(slave_cnt)) {
		slave_id = bond_rr_gen_slave_id(bond) % slave_cnt;
		return bond_get_slave_by_id(bond, slave_id);
	}
	return NULL;
}
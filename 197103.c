static bool bond_flow_dissect(struct bonding *bond, struct sk_buff *skb,
			      struct flow_keys *fk)
{
	bool l34 = bond->params.xmit_policy == BOND_XMIT_POLICY_LAYER34;
	int noff, proto = -1;

	switch (bond->params.xmit_policy) {
	case BOND_XMIT_POLICY_ENCAP23:
	case BOND_XMIT_POLICY_ENCAP34:
		memset(fk, 0, sizeof(*fk));
		return __skb_flow_dissect(NULL, skb, &flow_keys_bonding,
					  fk, NULL, 0, 0, 0, 0);
	default:
		break;
	}

	fk->ports.ports = 0;
	memset(&fk->icmp, 0, sizeof(fk->icmp));
	noff = skb_network_offset(skb);
	if (!bond_flow_ip(skb, fk, &noff, &proto, l34))
		return false;

	/* ICMP error packets contains at least 8 bytes of the header
	 * of the packet which generated the error. Use this information
	 * to correlate ICMP error packets within the same flow which
	 * generated the error.
	 */
	if (proto == IPPROTO_ICMP || proto == IPPROTO_ICMPV6) {
		skb_flow_get_icmp_tci(skb, &fk->icmp, skb->data,
				      skb_transport_offset(skb),
				      skb_headlen(skb));
		if (proto == IPPROTO_ICMP) {
			if (!icmp_is_err(fk->icmp.type))
				return true;

			noff += sizeof(struct icmphdr);
		} else if (proto == IPPROTO_ICMPV6) {
			if (!icmpv6_is_err(fk->icmp.type))
				return true;

			noff += sizeof(struct icmp6hdr);
		}
		return bond_flow_ip(skb, fk, &noff, &proto, l34);
	}

	return true;
}
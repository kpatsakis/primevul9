static int metadata_from_nlattrs(struct net *net, struct sw_flow_match *match,
				 u64 *attrs, const struct nlattr **a,
				 bool is_mask, bool log)
{
	u8 mac_proto = MAC_PROTO_ETHERNET;

	if (*attrs & (1 << OVS_KEY_ATTR_DP_HASH)) {
		u32 hash_val = nla_get_u32(a[OVS_KEY_ATTR_DP_HASH]);

		SW_FLOW_KEY_PUT(match, ovs_flow_hash, hash_val, is_mask);
		*attrs &= ~(1 << OVS_KEY_ATTR_DP_HASH);
	}

	if (*attrs & (1 << OVS_KEY_ATTR_RECIRC_ID)) {
		u32 recirc_id = nla_get_u32(a[OVS_KEY_ATTR_RECIRC_ID]);

		SW_FLOW_KEY_PUT(match, recirc_id, recirc_id, is_mask);
		*attrs &= ~(1 << OVS_KEY_ATTR_RECIRC_ID);
	}

	if (*attrs & (1 << OVS_KEY_ATTR_PRIORITY)) {
		SW_FLOW_KEY_PUT(match, phy.priority,
			  nla_get_u32(a[OVS_KEY_ATTR_PRIORITY]), is_mask);
		*attrs &= ~(1 << OVS_KEY_ATTR_PRIORITY);
	}

	if (*attrs & (1 << OVS_KEY_ATTR_IN_PORT)) {
		u32 in_port = nla_get_u32(a[OVS_KEY_ATTR_IN_PORT]);

		if (is_mask) {
			in_port = 0xffffffff; /* Always exact match in_port. */
		} else if (in_port >= DP_MAX_PORTS) {
			OVS_NLERR(log, "Port %d exceeds max allowable %d",
				  in_port, DP_MAX_PORTS);
			return -EINVAL;
		}

		SW_FLOW_KEY_PUT(match, phy.in_port, in_port, is_mask);
		*attrs &= ~(1 << OVS_KEY_ATTR_IN_PORT);
	} else if (!is_mask) {
		SW_FLOW_KEY_PUT(match, phy.in_port, DP_MAX_PORTS, is_mask);
	}

	if (*attrs & (1 << OVS_KEY_ATTR_SKB_MARK)) {
		uint32_t mark = nla_get_u32(a[OVS_KEY_ATTR_SKB_MARK]);

		SW_FLOW_KEY_PUT(match, phy.skb_mark, mark, is_mask);
		*attrs &= ~(1 << OVS_KEY_ATTR_SKB_MARK);
	}
	if (*attrs & (1 << OVS_KEY_ATTR_TUNNEL)) {
		if (ip_tun_from_nlattr(a[OVS_KEY_ATTR_TUNNEL], match,
				       is_mask, log) < 0)
			return -EINVAL;
		*attrs &= ~(1 << OVS_KEY_ATTR_TUNNEL);
	}

	if (*attrs & (1 << OVS_KEY_ATTR_CT_STATE) &&
	    ovs_ct_verify(net, OVS_KEY_ATTR_CT_STATE)) {
		u32 ct_state = nla_get_u32(a[OVS_KEY_ATTR_CT_STATE]);

		if (ct_state & ~CT_SUPPORTED_MASK) {
			OVS_NLERR(log, "ct_state flags %08x unsupported",
				  ct_state);
			return -EINVAL;
		}

		SW_FLOW_KEY_PUT(match, ct_state, ct_state, is_mask);
		*attrs &= ~(1ULL << OVS_KEY_ATTR_CT_STATE);
	}
	if (*attrs & (1 << OVS_KEY_ATTR_CT_ZONE) &&
	    ovs_ct_verify(net, OVS_KEY_ATTR_CT_ZONE)) {
		u16 ct_zone = nla_get_u16(a[OVS_KEY_ATTR_CT_ZONE]);

		SW_FLOW_KEY_PUT(match, ct_zone, ct_zone, is_mask);
		*attrs &= ~(1ULL << OVS_KEY_ATTR_CT_ZONE);
	}
	if (*attrs & (1 << OVS_KEY_ATTR_CT_MARK) &&
	    ovs_ct_verify(net, OVS_KEY_ATTR_CT_MARK)) {
		u32 mark = nla_get_u32(a[OVS_KEY_ATTR_CT_MARK]);

		SW_FLOW_KEY_PUT(match, ct.mark, mark, is_mask);
		*attrs &= ~(1ULL << OVS_KEY_ATTR_CT_MARK);
	}
	if (*attrs & (1 << OVS_KEY_ATTR_CT_LABELS) &&
	    ovs_ct_verify(net, OVS_KEY_ATTR_CT_LABELS)) {
		const struct ovs_key_ct_labels *cl;

		cl = nla_data(a[OVS_KEY_ATTR_CT_LABELS]);
		SW_FLOW_KEY_MEMCPY(match, ct.labels, cl->ct_labels,
				   sizeof(*cl), is_mask);
		*attrs &= ~(1ULL << OVS_KEY_ATTR_CT_LABELS);
	}
	if (*attrs & (1ULL << OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV4)) {
		const struct ovs_key_ct_tuple_ipv4 *ct;

		ct = nla_data(a[OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV4]);

		SW_FLOW_KEY_PUT(match, ipv4.ct_orig.src, ct->ipv4_src, is_mask);
		SW_FLOW_KEY_PUT(match, ipv4.ct_orig.dst, ct->ipv4_dst, is_mask);
		SW_FLOW_KEY_PUT(match, ct.orig_tp.src, ct->src_port, is_mask);
		SW_FLOW_KEY_PUT(match, ct.orig_tp.dst, ct->dst_port, is_mask);
		SW_FLOW_KEY_PUT(match, ct_orig_proto, ct->ipv4_proto, is_mask);
		*attrs &= ~(1ULL << OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV4);
	}
	if (*attrs & (1ULL << OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV6)) {
		const struct ovs_key_ct_tuple_ipv6 *ct;

		ct = nla_data(a[OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV6]);

		SW_FLOW_KEY_MEMCPY(match, ipv6.ct_orig.src, &ct->ipv6_src,
				   sizeof(match->key->ipv6.ct_orig.src),
				   is_mask);
		SW_FLOW_KEY_MEMCPY(match, ipv6.ct_orig.dst, &ct->ipv6_dst,
				   sizeof(match->key->ipv6.ct_orig.dst),
				   is_mask);
		SW_FLOW_KEY_PUT(match, ct.orig_tp.src, ct->src_port, is_mask);
		SW_FLOW_KEY_PUT(match, ct.orig_tp.dst, ct->dst_port, is_mask);
		SW_FLOW_KEY_PUT(match, ct_orig_proto, ct->ipv6_proto, is_mask);
		*attrs &= ~(1ULL << OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV6);
	}

	/* For layer 3 packets the Ethernet type is provided
	 * and treated as metadata but no MAC addresses are provided.
	 */
	if (!(*attrs & (1ULL << OVS_KEY_ATTR_ETHERNET)) &&
	    (*attrs & (1ULL << OVS_KEY_ATTR_ETHERTYPE)))
		mac_proto = MAC_PROTO_NONE;

	/* Always exact match mac_proto */
	SW_FLOW_KEY_PUT(match, mac_proto, is_mask ? 0xff : mac_proto, is_mask);

	if (mac_proto == MAC_PROTO_NONE)
		return parse_eth_type_from_nlattrs(match, attrs, a, is_mask,
						   log);

	return 0;
}
static bool match_validate(const struct sw_flow_match *match,
			   u64 key_attrs, u64 mask_attrs, bool log)
{
	u64 key_expected = 0;
	u64 mask_allowed = key_attrs;  /* At most allow all key attributes */

	/* The following mask attributes allowed only if they
	 * pass the validation tests. */
	mask_allowed &= ~((1 << OVS_KEY_ATTR_IPV4)
			| (1 << OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV4)
			| (1 << OVS_KEY_ATTR_IPV6)
			| (1 << OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV6)
			| (1 << OVS_KEY_ATTR_TCP)
			| (1 << OVS_KEY_ATTR_TCP_FLAGS)
			| (1 << OVS_KEY_ATTR_UDP)
			| (1 << OVS_KEY_ATTR_SCTP)
			| (1 << OVS_KEY_ATTR_ICMP)
			| (1 << OVS_KEY_ATTR_ICMPV6)
			| (1 << OVS_KEY_ATTR_ARP)
			| (1 << OVS_KEY_ATTR_ND)
			| (1 << OVS_KEY_ATTR_MPLS)
			| (1 << OVS_KEY_ATTR_NSH));

	/* Always allowed mask fields. */
	mask_allowed |= ((1 << OVS_KEY_ATTR_TUNNEL)
		       | (1 << OVS_KEY_ATTR_IN_PORT)
		       | (1 << OVS_KEY_ATTR_ETHERTYPE));

	/* Check key attributes. */
	if (match->key->eth.type == htons(ETH_P_ARP)
			|| match->key->eth.type == htons(ETH_P_RARP)) {
		key_expected |= 1 << OVS_KEY_ATTR_ARP;
		if (match->mask && (match->mask->key.eth.type == htons(0xffff)))
			mask_allowed |= 1 << OVS_KEY_ATTR_ARP;
	}

	if (eth_p_mpls(match->key->eth.type)) {
		key_expected |= 1 << OVS_KEY_ATTR_MPLS;
		if (match->mask && (match->mask->key.eth.type == htons(0xffff)))
			mask_allowed |= 1 << OVS_KEY_ATTR_MPLS;
	}

	if (match->key->eth.type == htons(ETH_P_IP)) {
		key_expected |= 1 << OVS_KEY_ATTR_IPV4;
		if (match->mask && match->mask->key.eth.type == htons(0xffff)) {
			mask_allowed |= 1 << OVS_KEY_ATTR_IPV4;
			mask_allowed |= 1 << OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV4;
		}

		if (match->key->ip.frag != OVS_FRAG_TYPE_LATER) {
			if (match->key->ip.proto == IPPROTO_UDP) {
				key_expected |= 1 << OVS_KEY_ATTR_UDP;
				if (match->mask && (match->mask->key.ip.proto == 0xff))
					mask_allowed |= 1 << OVS_KEY_ATTR_UDP;
			}

			if (match->key->ip.proto == IPPROTO_SCTP) {
				key_expected |= 1 << OVS_KEY_ATTR_SCTP;
				if (match->mask && (match->mask->key.ip.proto == 0xff))
					mask_allowed |= 1 << OVS_KEY_ATTR_SCTP;
			}

			if (match->key->ip.proto == IPPROTO_TCP) {
				key_expected |= 1 << OVS_KEY_ATTR_TCP;
				key_expected |= 1 << OVS_KEY_ATTR_TCP_FLAGS;
				if (match->mask && (match->mask->key.ip.proto == 0xff)) {
					mask_allowed |= 1 << OVS_KEY_ATTR_TCP;
					mask_allowed |= 1 << OVS_KEY_ATTR_TCP_FLAGS;
				}
			}

			if (match->key->ip.proto == IPPROTO_ICMP) {
				key_expected |= 1 << OVS_KEY_ATTR_ICMP;
				if (match->mask && (match->mask->key.ip.proto == 0xff))
					mask_allowed |= 1 << OVS_KEY_ATTR_ICMP;
			}
		}
	}

	if (match->key->eth.type == htons(ETH_P_IPV6)) {
		key_expected |= 1 << OVS_KEY_ATTR_IPV6;
		if (match->mask && match->mask->key.eth.type == htons(0xffff)) {
			mask_allowed |= 1 << OVS_KEY_ATTR_IPV6;
			mask_allowed |= 1 << OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV6;
		}

		if (match->key->ip.frag != OVS_FRAG_TYPE_LATER) {
			if (match->key->ip.proto == IPPROTO_UDP) {
				key_expected |= 1 << OVS_KEY_ATTR_UDP;
				if (match->mask && (match->mask->key.ip.proto == 0xff))
					mask_allowed |= 1 << OVS_KEY_ATTR_UDP;
			}

			if (match->key->ip.proto == IPPROTO_SCTP) {
				key_expected |= 1 << OVS_KEY_ATTR_SCTP;
				if (match->mask && (match->mask->key.ip.proto == 0xff))
					mask_allowed |= 1 << OVS_KEY_ATTR_SCTP;
			}

			if (match->key->ip.proto == IPPROTO_TCP) {
				key_expected |= 1 << OVS_KEY_ATTR_TCP;
				key_expected |= 1 << OVS_KEY_ATTR_TCP_FLAGS;
				if (match->mask && (match->mask->key.ip.proto == 0xff)) {
					mask_allowed |= 1 << OVS_KEY_ATTR_TCP;
					mask_allowed |= 1 << OVS_KEY_ATTR_TCP_FLAGS;
				}
			}

			if (match->key->ip.proto == IPPROTO_ICMPV6) {
				key_expected |= 1 << OVS_KEY_ATTR_ICMPV6;
				if (match->mask && (match->mask->key.ip.proto == 0xff))
					mask_allowed |= 1 << OVS_KEY_ATTR_ICMPV6;

				if (match->key->tp.src ==
						htons(NDISC_NEIGHBOUR_SOLICITATION) ||
				    match->key->tp.src == htons(NDISC_NEIGHBOUR_ADVERTISEMENT)) {
					key_expected |= 1 << OVS_KEY_ATTR_ND;
					/* Original direction conntrack tuple
					 * uses the same space as the ND fields
					 * in the key, so both are not allowed
					 * at the same time.
					 */
					mask_allowed &= ~(1ULL << OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV6);
					if (match->mask && (match->mask->key.tp.src == htons(0xff)))
						mask_allowed |= 1 << OVS_KEY_ATTR_ND;
				}
			}
		}
	}

	if (match->key->eth.type == htons(ETH_P_NSH)) {
		key_expected |= 1 << OVS_KEY_ATTR_NSH;
		if (match->mask &&
		    match->mask->key.eth.type == htons(0xffff)) {
			mask_allowed |= 1 << OVS_KEY_ATTR_NSH;
		}
	}

	if ((key_attrs & key_expected) != key_expected) {
		/* Key attributes check failed. */
		OVS_NLERR(log, "Missing key (keys=%llx, expected=%llx)",
			  (unsigned long long)key_attrs,
			  (unsigned long long)key_expected);
		return false;
	}

	if ((mask_attrs & mask_allowed) != mask_attrs) {
		/* Mask attributes check failed. */
		OVS_NLERR(log, "Unexpected mask (mask=%llx, allowed=%llx)",
			  (unsigned long long)mask_attrs,
			  (unsigned long long)mask_allowed);
		return false;
	}

	return true;
}
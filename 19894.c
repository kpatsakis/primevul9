static int ip_tun_from_nlattr(const struct nlattr *attr,
			      struct sw_flow_match *match, bool is_mask,
			      bool log)
{
	bool ttl = false, ipv4 = false, ipv6 = false;
	bool info_bridge_mode = false;
	__be16 tun_flags = 0;
	int opts_type = 0;
	struct nlattr *a;
	int rem;

	nla_for_each_nested(a, attr, rem) {
		int type = nla_type(a);
		int err;

		if (type > OVS_TUNNEL_KEY_ATTR_MAX) {
			OVS_NLERR(log, "Tunnel attr %d out of range max %d",
				  type, OVS_TUNNEL_KEY_ATTR_MAX);
			return -EINVAL;
		}

		if (!check_attr_len(nla_len(a),
				    ovs_tunnel_key_lens[type].len)) {
			OVS_NLERR(log, "Tunnel attr %d has unexpected len %d expected %d",
				  type, nla_len(a), ovs_tunnel_key_lens[type].len);
			return -EINVAL;
		}

		switch (type) {
		case OVS_TUNNEL_KEY_ATTR_ID:
			SW_FLOW_KEY_PUT(match, tun_key.tun_id,
					nla_get_be64(a), is_mask);
			tun_flags |= TUNNEL_KEY;
			break;
		case OVS_TUNNEL_KEY_ATTR_IPV4_SRC:
			SW_FLOW_KEY_PUT(match, tun_key.u.ipv4.src,
					nla_get_in_addr(a), is_mask);
			ipv4 = true;
			break;
		case OVS_TUNNEL_KEY_ATTR_IPV4_DST:
			SW_FLOW_KEY_PUT(match, tun_key.u.ipv4.dst,
					nla_get_in_addr(a), is_mask);
			ipv4 = true;
			break;
		case OVS_TUNNEL_KEY_ATTR_IPV6_SRC:
			SW_FLOW_KEY_PUT(match, tun_key.u.ipv6.src,
					nla_get_in6_addr(a), is_mask);
			ipv6 = true;
			break;
		case OVS_TUNNEL_KEY_ATTR_IPV6_DST:
			SW_FLOW_KEY_PUT(match, tun_key.u.ipv6.dst,
					nla_get_in6_addr(a), is_mask);
			ipv6 = true;
			break;
		case OVS_TUNNEL_KEY_ATTR_TOS:
			SW_FLOW_KEY_PUT(match, tun_key.tos,
					nla_get_u8(a), is_mask);
			break;
		case OVS_TUNNEL_KEY_ATTR_TTL:
			SW_FLOW_KEY_PUT(match, tun_key.ttl,
					nla_get_u8(a), is_mask);
			ttl = true;
			break;
		case OVS_TUNNEL_KEY_ATTR_DONT_FRAGMENT:
			tun_flags |= TUNNEL_DONT_FRAGMENT;
			break;
		case OVS_TUNNEL_KEY_ATTR_CSUM:
			tun_flags |= TUNNEL_CSUM;
			break;
		case OVS_TUNNEL_KEY_ATTR_TP_SRC:
			SW_FLOW_KEY_PUT(match, tun_key.tp_src,
					nla_get_be16(a), is_mask);
			break;
		case OVS_TUNNEL_KEY_ATTR_TP_DST:
			SW_FLOW_KEY_PUT(match, tun_key.tp_dst,
					nla_get_be16(a), is_mask);
			break;
		case OVS_TUNNEL_KEY_ATTR_OAM:
			tun_flags |= TUNNEL_OAM;
			break;
		case OVS_TUNNEL_KEY_ATTR_GENEVE_OPTS:
			if (opts_type) {
				OVS_NLERR(log, "Multiple metadata blocks provided");
				return -EINVAL;
			}

			err = genev_tun_opt_from_nlattr(a, match, is_mask, log);
			if (err)
				return err;

			tun_flags |= TUNNEL_GENEVE_OPT;
			opts_type = type;
			break;
		case OVS_TUNNEL_KEY_ATTR_VXLAN_OPTS:
			if (opts_type) {
				OVS_NLERR(log, "Multiple metadata blocks provided");
				return -EINVAL;
			}

			err = vxlan_tun_opt_from_nlattr(a, match, is_mask, log);
			if (err)
				return err;

			tun_flags |= TUNNEL_VXLAN_OPT;
			opts_type = type;
			break;
		case OVS_TUNNEL_KEY_ATTR_PAD:
			break;
		case OVS_TUNNEL_KEY_ATTR_ERSPAN_OPTS:
			if (opts_type) {
				OVS_NLERR(log, "Multiple metadata blocks provided");
				return -EINVAL;
			}

			err = erspan_tun_opt_from_nlattr(a, match, is_mask,
							 log);
			if (err)
				return err;

			tun_flags |= TUNNEL_ERSPAN_OPT;
			opts_type = type;
			break;
		case OVS_TUNNEL_KEY_ATTR_IPV4_INFO_BRIDGE:
			info_bridge_mode = true;
			ipv4 = true;
			break;
		default:
			OVS_NLERR(log, "Unknown IP tunnel attribute %d",
				  type);
			return -EINVAL;
		}
	}

	SW_FLOW_KEY_PUT(match, tun_key.tun_flags, tun_flags, is_mask);
	if (is_mask)
		SW_FLOW_KEY_MEMSET_FIELD(match, tun_proto, 0xff, true);
	else
		SW_FLOW_KEY_PUT(match, tun_proto, ipv6 ? AF_INET6 : AF_INET,
				false);

	if (rem > 0) {
		OVS_NLERR(log, "IP tunnel attribute has %d unknown bytes.",
			  rem);
		return -EINVAL;
	}

	if (ipv4 && ipv6) {
		OVS_NLERR(log, "Mixed IPv4 and IPv6 tunnel attributes");
		return -EINVAL;
	}

	if (!is_mask) {
		if (!ipv4 && !ipv6) {
			OVS_NLERR(log, "IP tunnel dst address not specified");
			return -EINVAL;
		}
		if (ipv4) {
			if (info_bridge_mode) {
				if (match->key->tun_key.u.ipv4.src ||
				    match->key->tun_key.u.ipv4.dst ||
				    match->key->tun_key.tp_src ||
				    match->key->tun_key.tp_dst ||
				    match->key->tun_key.ttl ||
				    match->key->tun_key.tos ||
				    tun_flags & ~TUNNEL_KEY) {
					OVS_NLERR(log, "IPv4 tun info is not correct");
					return -EINVAL;
				}
			} else if (!match->key->tun_key.u.ipv4.dst) {
				OVS_NLERR(log, "IPv4 tunnel dst address is zero");
				return -EINVAL;
			}
		}
		if (ipv6 && ipv6_addr_any(&match->key->tun_key.u.ipv6.dst)) {
			OVS_NLERR(log, "IPv6 tunnel dst address is zero");
			return -EINVAL;
		}

		if (!ttl && !info_bridge_mode) {
			OVS_NLERR(log, "IP tunnel TTL not specified.");
			return -EINVAL;
		}
	}

	return opts_type;
}
static int i40e_parse_cls_flower(struct i40e_vsi *vsi,
				 struct flow_cls_offload *f,
				 struct i40e_cloud_filter *filter)
{
	struct flow_rule *rule = flow_cls_offload_flow_rule(f);
	struct flow_dissector *dissector = rule->match.dissector;
	u16 n_proto_mask = 0, n_proto_key = 0, addr_type = 0;
	struct i40e_pf *pf = vsi->back;
	u8 field_flags = 0;

	if (dissector->used_keys &
	    ~(BIT(FLOW_DISSECTOR_KEY_CONTROL) |
	      BIT(FLOW_DISSECTOR_KEY_BASIC) |
	      BIT(FLOW_DISSECTOR_KEY_ETH_ADDRS) |
	      BIT(FLOW_DISSECTOR_KEY_VLAN) |
	      BIT(FLOW_DISSECTOR_KEY_IPV4_ADDRS) |
	      BIT(FLOW_DISSECTOR_KEY_IPV6_ADDRS) |
	      BIT(FLOW_DISSECTOR_KEY_PORTS) |
	      BIT(FLOW_DISSECTOR_KEY_ENC_KEYID))) {
		dev_err(&pf->pdev->dev, "Unsupported key used: 0x%x\n",
			dissector->used_keys);
		return -EOPNOTSUPP;
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_ENC_KEYID)) {
		struct flow_match_enc_keyid match;

		flow_rule_match_enc_keyid(rule, &match);
		if (match.mask->keyid != 0)
			field_flags |= I40E_CLOUD_FIELD_TEN_ID;

		filter->tenant_id = be32_to_cpu(match.key->keyid);
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_BASIC)) {
		struct flow_match_basic match;

		flow_rule_match_basic(rule, &match);
		n_proto_key = ntohs(match.key->n_proto);
		n_proto_mask = ntohs(match.mask->n_proto);

		if (n_proto_key == ETH_P_ALL) {
			n_proto_key = 0;
			n_proto_mask = 0;
		}
		filter->n_proto = n_proto_key & n_proto_mask;
		filter->ip_proto = match.key->ip_proto;
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_ETH_ADDRS)) {
		struct flow_match_eth_addrs match;

		flow_rule_match_eth_addrs(rule, &match);

		/* use is_broadcast and is_zero to check for all 0xf or 0 */
		if (!is_zero_ether_addr(match.mask->dst)) {
			if (is_broadcast_ether_addr(match.mask->dst)) {
				field_flags |= I40E_CLOUD_FIELD_OMAC;
			} else {
				dev_err(&pf->pdev->dev, "Bad ether dest mask %pM\n",
					match.mask->dst);
				return I40E_ERR_CONFIG;
			}
		}

		if (!is_zero_ether_addr(match.mask->src)) {
			if (is_broadcast_ether_addr(match.mask->src)) {
				field_flags |= I40E_CLOUD_FIELD_IMAC;
			} else {
				dev_err(&pf->pdev->dev, "Bad ether src mask %pM\n",
					match.mask->src);
				return I40E_ERR_CONFIG;
			}
		}
		ether_addr_copy(filter->dst_mac, match.key->dst);
		ether_addr_copy(filter->src_mac, match.key->src);
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_VLAN)) {
		struct flow_match_vlan match;

		flow_rule_match_vlan(rule, &match);
		if (match.mask->vlan_id) {
			if (match.mask->vlan_id == VLAN_VID_MASK) {
				field_flags |= I40E_CLOUD_FIELD_IVLAN;

			} else {
				dev_err(&pf->pdev->dev, "Bad vlan mask 0x%04x\n",
					match.mask->vlan_id);
				return I40E_ERR_CONFIG;
			}
		}

		filter->vlan_id = cpu_to_be16(match.key->vlan_id);
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_CONTROL)) {
		struct flow_match_control match;

		flow_rule_match_control(rule, &match);
		addr_type = match.key->addr_type;
	}

	if (addr_type == FLOW_DISSECTOR_KEY_IPV4_ADDRS) {
		struct flow_match_ipv4_addrs match;

		flow_rule_match_ipv4_addrs(rule, &match);
		if (match.mask->dst) {
			if (match.mask->dst == cpu_to_be32(0xffffffff)) {
				field_flags |= I40E_CLOUD_FIELD_IIP;
			} else {
				dev_err(&pf->pdev->dev, "Bad ip dst mask %pI4b\n",
					&match.mask->dst);
				return I40E_ERR_CONFIG;
			}
		}

		if (match.mask->src) {
			if (match.mask->src == cpu_to_be32(0xffffffff)) {
				field_flags |= I40E_CLOUD_FIELD_IIP;
			} else {
				dev_err(&pf->pdev->dev, "Bad ip src mask %pI4b\n",
					&match.mask->src);
				return I40E_ERR_CONFIG;
			}
		}

		if (field_flags & I40E_CLOUD_FIELD_TEN_ID) {
			dev_err(&pf->pdev->dev, "Tenant id not allowed for ip filter\n");
			return I40E_ERR_CONFIG;
		}
		filter->dst_ipv4 = match.key->dst;
		filter->src_ipv4 = match.key->src;
	}

	if (addr_type == FLOW_DISSECTOR_KEY_IPV6_ADDRS) {
		struct flow_match_ipv6_addrs match;

		flow_rule_match_ipv6_addrs(rule, &match);

		/* src and dest IPV6 address should not be LOOPBACK
		 * (0:0:0:0:0:0:0:1), which can be represented as ::1
		 */
		if (ipv6_addr_loopback(&match.key->dst) ||
		    ipv6_addr_loopback(&match.key->src)) {
			dev_err(&pf->pdev->dev,
				"Bad ipv6, addr is LOOPBACK\n");
			return I40E_ERR_CONFIG;
		}
		if (!ipv6_addr_any(&match.mask->dst) ||
		    !ipv6_addr_any(&match.mask->src))
			field_flags |= I40E_CLOUD_FIELD_IIP;

		memcpy(&filter->src_ipv6, &match.key->src.s6_addr32,
		       sizeof(filter->src_ipv6));
		memcpy(&filter->dst_ipv6, &match.key->dst.s6_addr32,
		       sizeof(filter->dst_ipv6));
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_PORTS)) {
		struct flow_match_ports match;

		flow_rule_match_ports(rule, &match);
		if (match.mask->src) {
			if (match.mask->src == cpu_to_be16(0xffff)) {
				field_flags |= I40E_CLOUD_FIELD_IIP;
			} else {
				dev_err(&pf->pdev->dev, "Bad src port mask 0x%04x\n",
					be16_to_cpu(match.mask->src));
				return I40E_ERR_CONFIG;
			}
		}

		if (match.mask->dst) {
			if (match.mask->dst == cpu_to_be16(0xffff)) {
				field_flags |= I40E_CLOUD_FIELD_IIP;
			} else {
				dev_err(&pf->pdev->dev, "Bad dst port mask 0x%04x\n",
					be16_to_cpu(match.mask->dst));
				return I40E_ERR_CONFIG;
			}
		}

		filter->dst_port = match.key->dst;
		filter->src_port = match.key->src;

		switch (filter->ip_proto) {
		case IPPROTO_TCP:
		case IPPROTO_UDP:
			break;
		default:
			dev_err(&pf->pdev->dev,
				"Only UDP and TCP transport are supported\n");
			return -EINVAL;
		}
	}
	filter->flags = field_flags;
	return 0;
}
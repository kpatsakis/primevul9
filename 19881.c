static int __ovs_nla_copy_actions(struct net *net, const struct nlattr *attr,
				  const struct sw_flow_key *key,
				  struct sw_flow_actions **sfa,
				  __be16 eth_type, __be16 vlan_tci,
				  u32 mpls_label_count, bool log)
{
	u8 mac_proto = ovs_key_mac_proto(key);
	const struct nlattr *a;
	int rem, err;

	nla_for_each_nested(a, attr, rem) {
		/* Expected argument lengths, (u32)-1 for variable length. */
		static const u32 action_lens[OVS_ACTION_ATTR_MAX + 1] = {
			[OVS_ACTION_ATTR_OUTPUT] = sizeof(u32),
			[OVS_ACTION_ATTR_RECIRC] = sizeof(u32),
			[OVS_ACTION_ATTR_USERSPACE] = (u32)-1,
			[OVS_ACTION_ATTR_PUSH_MPLS] = sizeof(struct ovs_action_push_mpls),
			[OVS_ACTION_ATTR_POP_MPLS] = sizeof(__be16),
			[OVS_ACTION_ATTR_PUSH_VLAN] = sizeof(struct ovs_action_push_vlan),
			[OVS_ACTION_ATTR_POP_VLAN] = 0,
			[OVS_ACTION_ATTR_SET] = (u32)-1,
			[OVS_ACTION_ATTR_SET_MASKED] = (u32)-1,
			[OVS_ACTION_ATTR_SAMPLE] = (u32)-1,
			[OVS_ACTION_ATTR_HASH] = sizeof(struct ovs_action_hash),
			[OVS_ACTION_ATTR_CT] = (u32)-1,
			[OVS_ACTION_ATTR_CT_CLEAR] = 0,
			[OVS_ACTION_ATTR_TRUNC] = sizeof(struct ovs_action_trunc),
			[OVS_ACTION_ATTR_PUSH_ETH] = sizeof(struct ovs_action_push_eth),
			[OVS_ACTION_ATTR_POP_ETH] = 0,
			[OVS_ACTION_ATTR_PUSH_NSH] = (u32)-1,
			[OVS_ACTION_ATTR_POP_NSH] = 0,
			[OVS_ACTION_ATTR_METER] = sizeof(u32),
			[OVS_ACTION_ATTR_CLONE] = (u32)-1,
			[OVS_ACTION_ATTR_CHECK_PKT_LEN] = (u32)-1,
			[OVS_ACTION_ATTR_ADD_MPLS] = sizeof(struct ovs_action_add_mpls),
			[OVS_ACTION_ATTR_DEC_TTL] = (u32)-1,
		};
		const struct ovs_action_push_vlan *vlan;
		int type = nla_type(a);
		bool skip_copy;

		if (type > OVS_ACTION_ATTR_MAX ||
		    (action_lens[type] != nla_len(a) &&
		     action_lens[type] != (u32)-1))
			return -EINVAL;

		skip_copy = false;
		switch (type) {
		case OVS_ACTION_ATTR_UNSPEC:
			return -EINVAL;

		case OVS_ACTION_ATTR_USERSPACE:
			err = validate_userspace(a);
			if (err)
				return err;
			break;

		case OVS_ACTION_ATTR_OUTPUT:
			if (nla_get_u32(a) >= DP_MAX_PORTS)
				return -EINVAL;
			break;

		case OVS_ACTION_ATTR_TRUNC: {
			const struct ovs_action_trunc *trunc = nla_data(a);

			if (trunc->max_len < ETH_HLEN)
				return -EINVAL;
			break;
		}

		case OVS_ACTION_ATTR_HASH: {
			const struct ovs_action_hash *act_hash = nla_data(a);

			switch (act_hash->hash_alg) {
			case OVS_HASH_ALG_L4:
				break;
			default:
				return  -EINVAL;
			}

			break;
		}

		case OVS_ACTION_ATTR_POP_VLAN:
			if (mac_proto != MAC_PROTO_ETHERNET)
				return -EINVAL;
			vlan_tci = htons(0);
			break;

		case OVS_ACTION_ATTR_PUSH_VLAN:
			if (mac_proto != MAC_PROTO_ETHERNET)
				return -EINVAL;
			vlan = nla_data(a);
			if (!eth_type_vlan(vlan->vlan_tpid))
				return -EINVAL;
			if (!(vlan->vlan_tci & htons(VLAN_CFI_MASK)))
				return -EINVAL;
			vlan_tci = vlan->vlan_tci;
			break;

		case OVS_ACTION_ATTR_RECIRC:
			break;

		case OVS_ACTION_ATTR_ADD_MPLS: {
			const struct ovs_action_add_mpls *mpls = nla_data(a);

			if (!eth_p_mpls(mpls->mpls_ethertype))
				return -EINVAL;

			if (mpls->tun_flags & OVS_MPLS_L3_TUNNEL_FLAG_MASK) {
				if (vlan_tci & htons(VLAN_CFI_MASK) ||
				    (eth_type != htons(ETH_P_IP) &&
				     eth_type != htons(ETH_P_IPV6) &&
				     eth_type != htons(ETH_P_ARP) &&
				     eth_type != htons(ETH_P_RARP) &&
				     !eth_p_mpls(eth_type)))
					return -EINVAL;
				mpls_label_count++;
			} else {
				if (mac_proto == MAC_PROTO_ETHERNET) {
					mpls_label_count = 1;
					mac_proto = MAC_PROTO_NONE;
				} else {
					mpls_label_count++;
				}
			}
			eth_type = mpls->mpls_ethertype;
			break;
		}

		case OVS_ACTION_ATTR_PUSH_MPLS: {
			const struct ovs_action_push_mpls *mpls = nla_data(a);

			if (!eth_p_mpls(mpls->mpls_ethertype))
				return -EINVAL;
			/* Prohibit push MPLS other than to a white list
			 * for packets that have a known tag order.
			 */
			if (vlan_tci & htons(VLAN_CFI_MASK) ||
			    (eth_type != htons(ETH_P_IP) &&
			     eth_type != htons(ETH_P_IPV6) &&
			     eth_type != htons(ETH_P_ARP) &&
			     eth_type != htons(ETH_P_RARP) &&
			     !eth_p_mpls(eth_type)))
				return -EINVAL;
			eth_type = mpls->mpls_ethertype;
			mpls_label_count++;
			break;
		}

		case OVS_ACTION_ATTR_POP_MPLS: {
			__be16  proto;
			if (vlan_tci & htons(VLAN_CFI_MASK) ||
			    !eth_p_mpls(eth_type))
				return -EINVAL;

			/* Disallow subsequent L2.5+ set actions and mpls_pop
			 * actions once the last MPLS label in the packet is
			 * is popped as there is no check here to ensure that
			 * the new eth type is valid and thus set actions could
			 * write off the end of the packet or otherwise corrupt
			 * it.
			 *
			 * Support for these actions is planned using packet
			 * recirculation.
			 */
			proto = nla_get_be16(a);

			if (proto == htons(ETH_P_TEB) &&
			    mac_proto != MAC_PROTO_NONE)
				return -EINVAL;

			mpls_label_count--;

			if (!eth_p_mpls(proto) || !mpls_label_count)
				eth_type = htons(0);
			else
				eth_type =  proto;

			break;
		}

		case OVS_ACTION_ATTR_SET:
			err = validate_set(a, key, sfa,
					   &skip_copy, mac_proto, eth_type,
					   false, log);
			if (err)
				return err;
			break;

		case OVS_ACTION_ATTR_SET_MASKED:
			err = validate_set(a, key, sfa,
					   &skip_copy, mac_proto, eth_type,
					   true, log);
			if (err)
				return err;
			break;

		case OVS_ACTION_ATTR_SAMPLE: {
			bool last = nla_is_last(a, rem);

			err = validate_and_copy_sample(net, a, key, sfa,
						       eth_type, vlan_tci,
						       mpls_label_count,
						       log, last);
			if (err)
				return err;
			skip_copy = true;
			break;
		}

		case OVS_ACTION_ATTR_CT:
			err = ovs_ct_copy_action(net, a, key, sfa, log);
			if (err)
				return err;
			skip_copy = true;
			break;

		case OVS_ACTION_ATTR_CT_CLEAR:
			break;

		case OVS_ACTION_ATTR_PUSH_ETH:
			/* Disallow pushing an Ethernet header if one
			 * is already present */
			if (mac_proto != MAC_PROTO_NONE)
				return -EINVAL;
			mac_proto = MAC_PROTO_ETHERNET;
			break;

		case OVS_ACTION_ATTR_POP_ETH:
			if (mac_proto != MAC_PROTO_ETHERNET)
				return -EINVAL;
			if (vlan_tci & htons(VLAN_CFI_MASK))
				return -EINVAL;
			mac_proto = MAC_PROTO_NONE;
			break;

		case OVS_ACTION_ATTR_PUSH_NSH:
			if (mac_proto != MAC_PROTO_ETHERNET) {
				u8 next_proto;

				next_proto = tun_p_from_eth_p(eth_type);
				if (!next_proto)
					return -EINVAL;
			}
			mac_proto = MAC_PROTO_NONE;
			if (!validate_nsh(nla_data(a), false, true, true))
				return -EINVAL;
			break;

		case OVS_ACTION_ATTR_POP_NSH: {
			__be16 inner_proto;

			if (eth_type != htons(ETH_P_NSH))
				return -EINVAL;
			inner_proto = tun_p_to_eth_p(key->nsh.base.np);
			if (!inner_proto)
				return -EINVAL;
			if (key->nsh.base.np == TUN_P_ETHERNET)
				mac_proto = MAC_PROTO_ETHERNET;
			else
				mac_proto = MAC_PROTO_NONE;
			break;
		}

		case OVS_ACTION_ATTR_METER:
			/* Non-existent meters are simply ignored.  */
			break;

		case OVS_ACTION_ATTR_CLONE: {
			bool last = nla_is_last(a, rem);

			err = validate_and_copy_clone(net, a, key, sfa,
						      eth_type, vlan_tci,
						      mpls_label_count,
						      log, last);
			if (err)
				return err;
			skip_copy = true;
			break;
		}

		case OVS_ACTION_ATTR_CHECK_PKT_LEN: {
			bool last = nla_is_last(a, rem);

			err = validate_and_copy_check_pkt_len(net, a, key, sfa,
							      eth_type,
							      vlan_tci,
							      mpls_label_count,
							      log, last);
			if (err)
				return err;
			skip_copy = true;
			break;
		}

		case OVS_ACTION_ATTR_DEC_TTL:
			err = validate_and_copy_dec_ttl(net, a, key, sfa,
							eth_type, vlan_tci,
							mpls_label_count, log);
			if (err)
				return err;
			skip_copy = true;
			break;

		default:
			OVS_NLERR(log, "Unknown Action type %d", type);
			return -EINVAL;
		}
		if (!skip_copy) {
			err = copy_action(a, sfa, log);
			if (err)
				return err;
		}
	}

	if (rem > 0)
		return -EINVAL;

	return 0;
}
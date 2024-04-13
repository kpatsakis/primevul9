static int validate_and_copy_set_tun(const struct nlattr *attr,
				     struct sw_flow_actions **sfa, bool log)
{
	struct sw_flow_match match;
	struct sw_flow_key key;
	struct metadata_dst *tun_dst;
	struct ip_tunnel_info *tun_info;
	struct ovs_tunnel_info *ovs_tun;
	struct nlattr *a;
	int err = 0, start, opts_type;
	__be16 dst_opt_type;

	dst_opt_type = 0;
	ovs_match_init(&match, &key, true, NULL);
	opts_type = ip_tun_from_nlattr(nla_data(attr), &match, false, log);
	if (opts_type < 0)
		return opts_type;

	if (key.tun_opts_len) {
		switch (opts_type) {
		case OVS_TUNNEL_KEY_ATTR_GENEVE_OPTS:
			err = validate_geneve_opts(&key);
			if (err < 0)
				return err;
			dst_opt_type = TUNNEL_GENEVE_OPT;
			break;
		case OVS_TUNNEL_KEY_ATTR_VXLAN_OPTS:
			dst_opt_type = TUNNEL_VXLAN_OPT;
			break;
		case OVS_TUNNEL_KEY_ATTR_ERSPAN_OPTS:
			dst_opt_type = TUNNEL_ERSPAN_OPT;
			break;
		}
	}

	start = add_nested_action_start(sfa, OVS_ACTION_ATTR_SET, log);
	if (start < 0)
		return start;

	tun_dst = metadata_dst_alloc(key.tun_opts_len, METADATA_IP_TUNNEL,
				     GFP_KERNEL);

	if (!tun_dst)
		return -ENOMEM;

	err = dst_cache_init(&tun_dst->u.tun_info.dst_cache, GFP_KERNEL);
	if (err) {
		dst_release((struct dst_entry *)tun_dst);
		return err;
	}

	a = __add_action(sfa, OVS_KEY_ATTR_TUNNEL_INFO, NULL,
			 sizeof(*ovs_tun), log);
	if (IS_ERR(a)) {
		dst_release((struct dst_entry *)tun_dst);
		return PTR_ERR(a);
	}

	ovs_tun = nla_data(a);
	ovs_tun->tun_dst = tun_dst;

	tun_info = &tun_dst->u.tun_info;
	tun_info->mode = IP_TUNNEL_INFO_TX;
	if (key.tun_proto == AF_INET6)
		tun_info->mode |= IP_TUNNEL_INFO_IPV6;
	else if (key.tun_proto == AF_INET && key.tun_key.u.ipv4.dst == 0)
		tun_info->mode |= IP_TUNNEL_INFO_BRIDGE;
	tun_info->key = key.tun_key;

	/* We need to store the options in the action itself since
	 * everything else will go away after flow setup. We can append
	 * it to tun_info and then point there.
	 */
	ip_tunnel_info_opts_set(tun_info,
				TUN_METADATA_OPTS(&key, key.tun_opts_len),
				key.tun_opts_len, dst_opt_type);
	add_nested_action_end(*sfa, start);

	return err;
}
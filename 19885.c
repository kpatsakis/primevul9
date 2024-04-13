static int set_action_to_attr(const struct nlattr *a, struct sk_buff *skb)
{
	const struct nlattr *ovs_key = nla_data(a);
	int key_type = nla_type(ovs_key);
	struct nlattr *start;
	int err;

	switch (key_type) {
	case OVS_KEY_ATTR_TUNNEL_INFO: {
		struct ovs_tunnel_info *ovs_tun = nla_data(ovs_key);
		struct ip_tunnel_info *tun_info = &ovs_tun->tun_dst->u.tun_info;

		start = nla_nest_start_noflag(skb, OVS_ACTION_ATTR_SET);
		if (!start)
			return -EMSGSIZE;

		err =  ip_tun_to_nlattr(skb, &tun_info->key,
					ip_tunnel_info_opts(tun_info),
					tun_info->options_len,
					ip_tunnel_info_af(tun_info), tun_info->mode);
		if (err)
			return err;
		nla_nest_end(skb, start);
		break;
	}
	default:
		if (nla_put(skb, OVS_ACTION_ATTR_SET, nla_len(a), ovs_key))
			return -EMSGSIZE;
		break;
	}

	return 0;
}
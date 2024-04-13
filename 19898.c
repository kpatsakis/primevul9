static void ovs_nla_free_set_action(const struct nlattr *a)
{
	const struct nlattr *ovs_key = nla_data(a);
	struct ovs_tunnel_info *ovs_tun;

	switch (nla_type(ovs_key)) {
	case OVS_KEY_ATTR_TUNNEL_INFO:
		ovs_tun = nla_data(ovs_key);
		dst_release((struct dst_entry *)ovs_tun->tun_dst);
		break;
	}
}
static int vxlan_opt_to_nlattr(struct sk_buff *skb,
			       const void *tun_opts, int swkey_tun_opts_len)
{
	const struct vxlan_metadata *opts = tun_opts;
	struct nlattr *nla;

	nla = nla_nest_start_noflag(skb, OVS_TUNNEL_KEY_ATTR_VXLAN_OPTS);
	if (!nla)
		return -EMSGSIZE;

	if (nla_put_u32(skb, OVS_VXLAN_EXT_GBP, opts->gbp) < 0)
		return -EMSGSIZE;

	nla_nest_end(skb, nla);
	return 0;
}
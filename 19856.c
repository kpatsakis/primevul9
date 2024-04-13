static int ip_tun_to_nlattr(struct sk_buff *skb,
			    const struct ip_tunnel_key *output,
			    const void *tun_opts, int swkey_tun_opts_len,
			    unsigned short tun_proto, u8 mode)
{
	struct nlattr *nla;
	int err;

	nla = nla_nest_start_noflag(skb, OVS_KEY_ATTR_TUNNEL);
	if (!nla)
		return -EMSGSIZE;

	err = __ip_tun_to_nlattr(skb, output, tun_opts, swkey_tun_opts_len,
				 tun_proto, mode);
	if (err)
		return err;

	nla_nest_end(skb, nla);
	return 0;
}
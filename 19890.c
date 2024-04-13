int ovs_nla_put_tunnel_info(struct sk_buff *skb,
			    struct ip_tunnel_info *tun_info)
{
	return __ip_tun_to_nlattr(skb, &tun_info->key,
				  ip_tunnel_info_opts(tun_info),
				  tun_info->options_len,
				  ip_tunnel_info_af(tun_info), tun_info->mode);
}
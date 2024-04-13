static int inet6_rtm_newroute(struct sk_buff *skb, struct nlmsghdr* nlh)
{
	struct fib6_config cfg;
	int err;

	err = rtm_to_fib6_config(skb, nlh, &cfg);
	if (err < 0)
		return err;

	if (cfg.fc_mp)
		return ip6_route_multipath(&cfg, 1);
	else
		return ip6_route_add(&cfg);
}
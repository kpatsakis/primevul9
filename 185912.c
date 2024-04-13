int ip6_ins_rt(struct rt6_info *rt)
{
	struct nl_info info = {
		.nl_net = dev_net(rt->dst.dev),
	};
	return __ip6_ins_rt(rt, &info, NULL, 0);
}
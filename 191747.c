static void netlink_compare_arg_init(struct netlink_compare_arg *arg,
				     struct net *net, u32 portid)
{
	memset(arg, 0, sizeof(*arg));
	write_pnet(&arg->pnet, net);
	arg->portid = portid;
}
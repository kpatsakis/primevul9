static inline u32 netlink_hash(const void *data, u32 len, u32 seed)
{
	const struct netlink_sock *nlk = data;
	struct netlink_compare_arg arg;

	netlink_compare_arg_init(&arg, sock_net(&nlk->sk), nlk->portid);
	return jhash2((u32 *)&arg, netlink_compare_arg_len / sizeof(u32), seed);
}
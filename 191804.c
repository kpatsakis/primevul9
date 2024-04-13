static inline int netlink_compare(struct rhashtable_compare_arg *arg,
				  const void *ptr)
{
	const struct netlink_compare_arg *x = arg->key;
	const struct netlink_sock *nlk = ptr;

	return nlk->portid != x->portid ||
	       !net_eq(sock_net(&nlk->sk), read_pnet(&x->pnet));
}
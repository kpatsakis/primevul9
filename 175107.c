static void __rds_inc_msg_cp(struct rds_incoming *inc,
			     struct rds_info_iterator *iter,
			     void *saddr, void *daddr, int flip, bool isv6)
{
#if IS_ENABLED(CONFIG_IPV6)
	if (isv6)
		rds6_inc_info_copy(inc, iter, saddr, daddr, flip);
	else
#endif
		rds_inc_info_copy(inc, iter, *(__be32 *)saddr,
				  *(__be32 *)daddr, flip);
}
struct rds_connection *rds_conn_create_outgoing(struct net *net,
						const struct in6_addr *laddr,
						const struct in6_addr *faddr,
						struct rds_transport *trans,
						u8 tos, gfp_t gfp, int dev_if)
{
	return __rds_conn_create(net, laddr, faddr, trans, gfp, tos, 1, dev_if);
}
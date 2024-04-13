void rds_sock_addref(struct rds_sock *rs)
{
	sock_hold(rds_rs_to_sk(rs));
}
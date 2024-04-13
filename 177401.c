void rds_sock_put(struct rds_sock *rs)
{
	sock_put(rds_rs_to_sk(rs));
}
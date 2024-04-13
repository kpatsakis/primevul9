static void rds_sock_destruct(struct sock *sk)
{
	struct rds_sock *rs = rds_sk_to_rs(sk);

	WARN_ON((&rs->rs_item != rs->rs_item.next ||
		 &rs->rs_item != rs->rs_item.prev));
}
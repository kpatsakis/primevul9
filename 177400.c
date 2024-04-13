void rds_wake_sk_sleep(struct rds_sock *rs)
{
	unsigned long flags;

	read_lock_irqsave(&rs->rs_recv_lock, flags);
	__rds_wake_sk_sleep(rds_rs_to_sk(rs));
	read_unlock_irqrestore(&rs->rs_recv_lock, flags);
}
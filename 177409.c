static unsigned int rds_poll(struct file *file, struct socket *sock,
			     poll_table *wait)
{
	struct sock *sk = sock->sk;
	struct rds_sock *rs = rds_sk_to_rs(sk);
	unsigned int mask = 0;
	unsigned long flags;

	poll_wait(file, sk_sleep(sk), wait);

	if (rs->rs_seen_congestion)
		poll_wait(file, &rds_poll_waitq, wait);

	read_lock_irqsave(&rs->rs_recv_lock, flags);
	if (!rs->rs_cong_monitor) {
		/* When a congestion map was updated, we signal POLLIN for
		 * "historical" reasons. Applications can also poll for
		 * WRBAND instead. */
		if (rds_cong_updated_since(&rs->rs_cong_track))
			mask |= (POLLIN | POLLRDNORM | POLLWRBAND);
	} else {
		spin_lock(&rs->rs_lock);
		if (rs->rs_cong_notify)
			mask |= (POLLIN | POLLRDNORM);
		spin_unlock(&rs->rs_lock);
	}
	if (!list_empty(&rs->rs_recv_queue) ||
	    !list_empty(&rs->rs_notify_queue))
		mask |= (POLLIN | POLLRDNORM);
	if (rs->rs_snd_bytes < rds_sk_sndbuf(rs))
		mask |= (POLLOUT | POLLWRNORM);
	read_unlock_irqrestore(&rs->rs_recv_lock, flags);

	/* clear state any time we wake a seen-congested socket */
	if (mask)
		rs->rs_seen_congestion = 0;

	return mask;
}
static int dccp_wait_for_ccid(struct sock *sk, struct sk_buff *skb, int delay)
{
	struct dccp_sock *dp = dccp_sk(sk);
	DEFINE_WAIT(wait);
	unsigned long jiffdelay;
	int rc;

	do {
		dccp_pr_debug("delayed send by %d msec\n", delay);
		jiffdelay = msecs_to_jiffies(delay);

		prepare_to_wait(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);

		sk->sk_write_pending++;
		release_sock(sk);
		schedule_timeout(jiffdelay);
		lock_sock(sk);
		sk->sk_write_pending--;

		if (sk->sk_err)
			goto do_error;
		if (signal_pending(current))
			goto do_interrupted;

		rc = ccid_hc_tx_send_packet(dp->dccps_hc_tx_ccid, sk, skb);
	} while ((delay = rc) > 0);
out:
	finish_wait(sk->sk_sleep, &wait);
	return rc;

do_error:
	rc = -EPIPE;
	goto out;
do_interrupted:
	rc = -EINTR;
	goto out;
}
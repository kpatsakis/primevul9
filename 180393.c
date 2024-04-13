int dccp_ioctl(struct sock *sk, int cmd, unsigned long arg)
{
	int rc = -ENOTCONN;

	lock_sock(sk);

	if (sk->sk_state == DCCP_LISTEN)
		goto out;

	switch (cmd) {
	case SIOCINQ: {
		struct sk_buff *skb;
		unsigned long amount = 0;

		skb = skb_peek(&sk->sk_receive_queue);
		if (skb != NULL) {
			/*
			 * We will only return the amount of this packet since
			 * that is all that will be read.
			 */
			amount = skb->len;
		}
		rc = put_user(amount, (int __user *)arg);
	}
		break;
	default:
		rc = -ENOIOCTLCMD;
		break;
	}
out:
	release_sock(sk);
	return rc;
}
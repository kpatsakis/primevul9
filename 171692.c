struct sk_buff *sock_alloc_send_pskb(struct sock *sk, unsigned long header_len,
				     unsigned long data_len, int noblock,
				     int *errcode, int max_page_order)
{
	struct sk_buff *skb;
	long timeo;
	int err;

	timeo = sock_sndtimeo(sk, noblock);
	for (;;) {
		err = sock_error(sk);
		if (err != 0)
			goto failure;

		err = -EPIPE;
		if (sk->sk_shutdown & SEND_SHUTDOWN)
			goto failure;

		if (sk_wmem_alloc_get(sk) < sk->sk_sndbuf)
			break;

		sk_set_bit(SOCKWQ_ASYNC_NOSPACE, sk);
		set_bit(SOCK_NOSPACE, &sk->sk_socket->flags);
		err = -EAGAIN;
		if (!timeo)
			goto failure;
		if (signal_pending(current))
			goto interrupted;
		timeo = sock_wait_for_wmem(sk, timeo);
	}
	skb = alloc_skb_with_frags(header_len, data_len, max_page_order,
				   errcode, sk->sk_allocation);
	if (skb)
		skb_set_owner_w(skb, sk);
	return skb;

interrupted:
	err = sock_intr_errno(timeo);
failure:
	*errcode = err;
	return NULL;
}
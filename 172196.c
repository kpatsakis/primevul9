static int hidp_send_message(struct hidp_session *session, struct socket *sock,
			     struct sk_buff_head *transmit, unsigned char hdr,
			     const unsigned char *data, int size)
{
	struct sk_buff *skb;
	struct sock *sk = sock->sk;

	BT_DBG("session %p data %p size %d", session, data, size);

	if (atomic_read(&session->terminate))
		return -EIO;

	skb = alloc_skb(size + 1, GFP_ATOMIC);
	if (!skb) {
		BT_ERR("Can't allocate memory for new frame");
		return -ENOMEM;
	}

	skb_put_u8(skb, hdr);
	if (data && size > 0)
		skb_put_data(skb, data, size);

	skb_queue_tail(transmit, skb);
	wake_up_interruptible(sk_sleep(sk));

	return 0;
}
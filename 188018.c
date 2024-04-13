static int packet_recv_error(struct sock *sk, struct msghdr *msg, int len)
{
	struct sock_exterr_skb *serr;
	struct sk_buff *skb, *skb2;
	int copied, err;

	err = -EAGAIN;
	skb = skb_dequeue(&sk->sk_error_queue);
	if (skb == NULL)
		goto out;

	copied = skb->len;
	if (copied > len) {
		msg->msg_flags |= MSG_TRUNC;
		copied = len;
	}
	err = skb_copy_datagram_iovec(skb, 0, msg->msg_iov, copied);
	if (err)
		goto out_free_skb;

	sock_recv_timestamp(msg, sk, skb);

	serr = SKB_EXT_ERR(skb);
	put_cmsg(msg, SOL_PACKET, PACKET_TX_TIMESTAMP,
		 sizeof(serr->ee), &serr->ee);

	msg->msg_flags |= MSG_ERRQUEUE;
	err = copied;

	/* Reset and regenerate socket error */
	spin_lock_bh(&sk->sk_error_queue.lock);
	sk->sk_err = 0;
	if ((skb2 = skb_peek(&sk->sk_error_queue)) != NULL) {
		sk->sk_err = SKB_EXT_ERR(skb2)->ee.ee_errno;
		spin_unlock_bh(&sk->sk_error_queue.lock);
		sk->sk_error_report(sk);
	} else
		spin_unlock_bh(&sk->sk_error_queue.lock);

out_free_skb:
	kfree_skb(skb);
out:
	return err;
}
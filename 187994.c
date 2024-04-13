static unsigned int packet_poll(struct file *file, struct socket *sock,
				poll_table *wait)
{
	struct sock *sk = sock->sk;
	struct packet_sock *po = pkt_sk(sk);
	unsigned int mask = datagram_poll(file, sock, wait);

	spin_lock_bh(&sk->sk_receive_queue.lock);
	if (po->rx_ring.pg_vec) {
		if (!packet_previous_rx_frame(po, &po->rx_ring,
			TP_STATUS_KERNEL))
			mask |= POLLIN | POLLRDNORM;
	}
	spin_unlock_bh(&sk->sk_receive_queue.lock);
	spin_lock_bh(&sk->sk_write_queue.lock);
	if (po->tx_ring.pg_vec) {
		if (packet_current_frame(po, &po->tx_ring, TP_STATUS_AVAILABLE))
			mask |= POLLOUT | POLLWRNORM;
	}
	spin_unlock_bh(&sk->sk_write_queue.lock);
	return mask;
}
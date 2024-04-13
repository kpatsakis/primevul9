static int packet_sendmsg(struct kiocb *iocb, struct socket *sock,
		struct msghdr *msg, size_t len)
{
	struct sock *sk = sock->sk;
	struct packet_sock *po = pkt_sk(sk);
	if (po->tx_ring.pg_vec)
		return tpacket_snd(po, msg);
	else
		return packet_snd(sock, msg, len);
}
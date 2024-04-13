static int packet_bind_spkt(struct socket *sock, struct sockaddr *uaddr,
			    int addr_len)
{
	struct sock *sk = sock->sk;
	char name[15];

	/*
	 *	Check legality
	 */

	if (addr_len != sizeof(struct sockaddr))
		return -EINVAL;
	strlcpy(name, uaddr->sa_data, sizeof(name));

	return packet_do_bind(sk, name, 0, pkt_sk(sk)->num);
}
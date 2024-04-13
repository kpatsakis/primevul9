static int packet_bind_spkt(struct socket *sock, struct sockaddr *uaddr,
			    int addr_len)
{
	struct sock *sk = sock->sk;
	char name[15];
	struct net_device *dev;
	int err = -ENODEV;

	/*
	 *	Check legality
	 */

	if (addr_len != sizeof(struct sockaddr))
		return -EINVAL;
	strlcpy(name, uaddr->sa_data, sizeof(name));

	dev = dev_get_by_name(sock_net(sk), name);
	if (dev)
		err = packet_do_bind(sk, dev, pkt_sk(sk)->num);
	return err;
}
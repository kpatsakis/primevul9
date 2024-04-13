base_sock_bind(struct socket *sock, struct sockaddr *addr, int addr_len)
{
	struct sockaddr_mISDN *maddr = (struct sockaddr_mISDN *) addr;
	struct sock *sk = sock->sk;
	int err = 0;

	if (addr_len < sizeof(struct sockaddr_mISDN))
		return -EINVAL;

	if (!maddr || maddr->family != AF_ISDN)
		return -EINVAL;

	lock_sock(sk);

	if (_pms(sk)->dev) {
		err = -EALREADY;
		goto done;
	}

	_pms(sk)->dev = get_mdevice(maddr->dev);
	if (!_pms(sk)->dev) {
		err = -ENODEV;
		goto done;
	}
	sk->sk_state = MISDN_BOUND;

done:
	release_sock(sk);
	return err;
}
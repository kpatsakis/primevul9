data_sock_getname(struct socket *sock, struct sockaddr *addr,
		  int peer)
{
	struct sockaddr_mISDN	*maddr = (struct sockaddr_mISDN *) addr;
	struct sock		*sk = sock->sk;

	if (!_pms(sk)->dev)
		return -EBADFD;

	lock_sock(sk);

	maddr->family = AF_ISDN;
	maddr->dev = _pms(sk)->dev->id;
	maddr->channel = _pms(sk)->ch.nr;
	maddr->sapi = _pms(sk)->ch.addr & 0xff;
	maddr->tei = (_pms(sk)->ch.addr >> 8) & 0xff;
	release_sock(sk);
	return sizeof(*maddr);
}
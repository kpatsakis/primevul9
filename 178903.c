static int pptp_getname(struct socket *sock, struct sockaddr *uaddr,
	int *usockaddr_len, int peer)
{
	int len = sizeof(struct sockaddr_pppox);
	struct sockaddr_pppox sp;

	memset(&sp.sa_addr, 0, sizeof(sp.sa_addr));

	sp.sa_family    = AF_PPPOX;
	sp.sa_protocol  = PX_PROTO_PPTP;
	sp.sa_addr.pptp = pppox_sk(sock->sk)->proto.pptp.src_addr;

	memcpy(uaddr, &sp, len);

	*usockaddr_len = len;

	return 0;
}
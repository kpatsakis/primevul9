static int alg_accept(struct socket *sock, struct socket *newsock, int flags,
		      bool kern)
{
	return af_alg_accept(sock->sk, newsock, kern);
}
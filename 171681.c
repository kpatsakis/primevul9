int sock_no_getname(struct socket *sock, struct sockaddr *saddr,
		    int *len, int peer)
{
	return -EOPNOTSUPP;
}
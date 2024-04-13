int sock_no_setsockopt(struct socket *sock, int level, int optname,
		    char __user *optval, unsigned int optlen)
{
	return -EOPNOTSUPP;
}
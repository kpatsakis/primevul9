int sock_no_getsockopt(struct socket *sock, int level, int optname,
		    char __user *optval, int __user *optlen)
{
	return -EOPNOTSUPP;
}
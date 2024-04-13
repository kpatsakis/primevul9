static int data_sock_getsockopt(struct socket *sock, int level, int optname,
				char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	int len, opt;

	if (get_user(len, optlen))
		return -EFAULT;

	if (len != sizeof(char))
		return -EINVAL;

	switch (optname) {
	case MISDN_TIME_STAMP:
		if (_pms(sk)->cmask & MISDN_TIME_STAMP)
			opt = 1;
		else
			opt = 0;

		if (put_user(opt, optval))
			return -EFAULT;
		break;
	default:
		return -ENOPROTOOPT;
	}

	return 0;
}
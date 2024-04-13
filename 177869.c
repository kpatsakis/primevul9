static int data_sock_setsockopt(struct socket *sock, int level, int optname,
				char __user *optval, unsigned int len)
{
	struct sock *sk = sock->sk;
	int err = 0, opt = 0;

	if (*debug & DEBUG_SOCKET)
		printk(KERN_DEBUG "%s(%p, %d, %x, %p, %d)\n", __func__, sock,
		       level, optname, optval, len);

	lock_sock(sk);

	switch (optname) {
	case MISDN_TIME_STAMP:
		if (get_user(opt, (int __user *)optval)) {
			err = -EFAULT;
			break;
		}

		if (opt)
			_pms(sk)->cmask |= MISDN_TIME_STAMP;
		else
			_pms(sk)->cmask &= ~MISDN_TIME_STAMP;
		break;
	default:
		err = -ENOPROTOOPT;
		break;
	}
	release_sock(sk);
	return err;
}
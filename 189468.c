static int nfc_llcp_setsockopt(struct socket *sock, int level, int optname,
			       char __user *optval, unsigned int optlen)
{
	struct sock *sk = sock->sk;
	struct nfc_llcp_sock *llcp_sock = nfc_llcp_sock(sk);
	u32 opt;
	int err = 0;

	pr_debug("%p optname %d\n", sk, optname);

	if (level != SOL_NFC)
		return -ENOPROTOOPT;

	lock_sock(sk);

	switch (optname) {
	case NFC_LLCP_RW:
		if (sk->sk_state == LLCP_CONNECTED ||
		    sk->sk_state == LLCP_BOUND ||
		    sk->sk_state == LLCP_LISTEN) {
			err = -EINVAL;
			break;
		}

		if (get_user(opt, (u32 __user *) optval)) {
			err = -EFAULT;
			break;
		}

		if (opt > LLCP_MAX_RW) {
			err = -EINVAL;
			break;
		}

		llcp_sock->rw = (u8) opt;

		break;

	case NFC_LLCP_MIUX:
		if (sk->sk_state == LLCP_CONNECTED ||
		    sk->sk_state == LLCP_BOUND ||
		    sk->sk_state == LLCP_LISTEN) {
			err = -EINVAL;
			break;
		}

		if (get_user(opt, (u32 __user *) optval)) {
			err = -EFAULT;
			break;
		}

		if (opt > LLCP_MAX_MIUX) {
			err = -EINVAL;
			break;
		}

		llcp_sock->miux = cpu_to_be16((u16) opt);

		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);

	pr_debug("%p rw %d miux %d\n", llcp_sock,
		 llcp_sock->rw, llcp_sock->miux);

	return err;
}
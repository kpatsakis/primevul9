static int pptp_ppp_ioctl(struct ppp_channel *chan, unsigned int cmd,
	unsigned long arg)
{
	struct sock *sk = (struct sock *) chan->private;
	struct pppox_sock *po = pppox_sk(sk);
	struct pptp_opt *opt = &po->proto.pptp;
	void __user *argp = (void __user *)arg;
	int __user *p = argp;
	int err, val;

	err = -EFAULT;
	switch (cmd) {
	case PPPIOCGFLAGS:
		val = opt->ppp_flags;
		if (put_user(val, p))
			break;
		err = 0;
		break;
	case PPPIOCSFLAGS:
		if (get_user(val, p))
			break;
		opt->ppp_flags = val & ~SC_RCV_BITS;
		err = 0;
		break;
	default:
		err = -ENOTTY;
	}

	return err;
}
static int netlink_getsockopt(struct socket *sock, int level, int optname,
			      char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	struct netlink_sock *nlk = nlk_sk(sk);
	int len, val, err;

	if (level != SOL_NETLINK)
		return -ENOPROTOOPT;

	if (get_user(len, optlen))
		return -EFAULT;
	if (len < 0)
		return -EINVAL;

	switch (optname) {
	case NETLINK_PKTINFO:
		if (len < sizeof(int))
			return -EINVAL;
		len = sizeof(int);
		val = nlk->flags & NETLINK_F_RECV_PKTINFO ? 1 : 0;
		if (put_user(len, optlen) ||
		    put_user(val, optval))
			return -EFAULT;
		err = 0;
		break;
	case NETLINK_BROADCAST_ERROR:
		if (len < sizeof(int))
			return -EINVAL;
		len = sizeof(int);
		val = nlk->flags & NETLINK_F_BROADCAST_SEND_ERROR ? 1 : 0;
		if (put_user(len, optlen) ||
		    put_user(val, optval))
			return -EFAULT;
		err = 0;
		break;
	case NETLINK_NO_ENOBUFS:
		if (len < sizeof(int))
			return -EINVAL;
		len = sizeof(int);
		val = nlk->flags & NETLINK_F_RECV_NO_ENOBUFS ? 1 : 0;
		if (put_user(len, optlen) ||
		    put_user(val, optval))
			return -EFAULT;
		err = 0;
		break;
	case NETLINK_LIST_MEMBERSHIPS: {
		int pos, idx, shift;

		err = 0;
		netlink_lock_table();
		for (pos = 0; pos * 8 < nlk->ngroups; pos += sizeof(u32)) {
			if (len - pos < sizeof(u32))
				break;

			idx = pos / sizeof(unsigned long);
			shift = (pos % sizeof(unsigned long)) * 8;
			if (put_user((u32)(nlk->groups[idx] >> shift),
				     (u32 __user *)(optval + pos))) {
				err = -EFAULT;
				break;
			}
		}
		if (put_user(ALIGN(nlk->ngroups / 8, sizeof(u32)), optlen))
			err = -EFAULT;
		netlink_unlock_table();
		break;
	}
	case NETLINK_CAP_ACK:
		if (len < sizeof(int))
			return -EINVAL;
		len = sizeof(int);
		val = nlk->flags & NETLINK_F_CAP_ACK ? 1 : 0;
		if (put_user(len, optlen) ||
		    put_user(val, optval))
			return -EFAULT;
		err = 0;
		break;
	case NETLINK_EXT_ACK:
		if (len < sizeof(int))
			return -EINVAL;
		len = sizeof(int);
		val = nlk->flags & NETLINK_F_EXT_ACK ? 1 : 0;
		if (put_user(len, optlen) || put_user(val, optval))
			return -EFAULT;
		err = 0;
		break;
	default:
		err = -ENOPROTOOPT;
	}
	return err;
}
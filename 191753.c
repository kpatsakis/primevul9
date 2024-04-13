static int netlink_setsockopt(struct socket *sock, int level, int optname,
			      char __user *optval, unsigned int optlen)
{
	struct sock *sk = sock->sk;
	struct netlink_sock *nlk = nlk_sk(sk);
	unsigned int val = 0;
	int err;

	if (level != SOL_NETLINK)
		return -ENOPROTOOPT;

	if (optlen >= sizeof(int) &&
	    get_user(val, (unsigned int __user *)optval))
		return -EFAULT;

	switch (optname) {
	case NETLINK_PKTINFO:
		if (val)
			nlk->flags |= NETLINK_F_RECV_PKTINFO;
		else
			nlk->flags &= ~NETLINK_F_RECV_PKTINFO;
		err = 0;
		break;
	case NETLINK_ADD_MEMBERSHIP:
	case NETLINK_DROP_MEMBERSHIP: {
		if (!netlink_allowed(sock, NL_CFG_F_NONROOT_RECV))
			return -EPERM;
		err = netlink_realloc_groups(sk);
		if (err)
			return err;
		if (!val || val - 1 >= nlk->ngroups)
			return -EINVAL;
		if (optname == NETLINK_ADD_MEMBERSHIP && nlk->netlink_bind) {
			err = nlk->netlink_bind(sock_net(sk), val);
			if (err)
				return err;
		}
		netlink_table_grab();
		netlink_update_socket_mc(nlk, val,
					 optname == NETLINK_ADD_MEMBERSHIP);
		netlink_table_ungrab();
		if (optname == NETLINK_DROP_MEMBERSHIP && nlk->netlink_unbind)
			nlk->netlink_unbind(sock_net(sk), val);

		err = 0;
		break;
	}
	case NETLINK_BROADCAST_ERROR:
		if (val)
			nlk->flags |= NETLINK_F_BROADCAST_SEND_ERROR;
		else
			nlk->flags &= ~NETLINK_F_BROADCAST_SEND_ERROR;
		err = 0;
		break;
	case NETLINK_NO_ENOBUFS:
		if (val) {
			nlk->flags |= NETLINK_F_RECV_NO_ENOBUFS;
			clear_bit(NETLINK_S_CONGESTED, &nlk->state);
			wake_up_interruptible(&nlk->wait);
		} else {
			nlk->flags &= ~NETLINK_F_RECV_NO_ENOBUFS;
		}
		err = 0;
		break;
	case NETLINK_LISTEN_ALL_NSID:
		if (!ns_capable(sock_net(sk)->user_ns, CAP_NET_BROADCAST))
			return -EPERM;

		if (val)
			nlk->flags |= NETLINK_F_LISTEN_ALL_NSID;
		else
			nlk->flags &= ~NETLINK_F_LISTEN_ALL_NSID;
		err = 0;
		break;
	case NETLINK_CAP_ACK:
		if (val)
			nlk->flags |= NETLINK_F_CAP_ACK;
		else
			nlk->flags &= ~NETLINK_F_CAP_ACK;
		err = 0;
		break;
	case NETLINK_EXT_ACK:
		if (val)
			nlk->flags |= NETLINK_F_EXT_ACK;
		else
			nlk->flags &= ~NETLINK_F_EXT_ACK;
		err = 0;
		break;
	default:
		err = -ENOPROTOOPT;
	}
	return err;
}
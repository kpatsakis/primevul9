static int do_dccp_setsockopt(struct sock *sk, int level, int optname,
		char __user *optval, unsigned int optlen)
{
	struct dccp_sock *dp = dccp_sk(sk);
	int val, err = 0;

	switch (optname) {
	case DCCP_SOCKOPT_PACKET_SIZE:
		DCCP_WARN("sockopt(PACKET_SIZE) is deprecated: fix your app\n");
		return 0;
	case DCCP_SOCKOPT_CHANGE_L:
	case DCCP_SOCKOPT_CHANGE_R:
		DCCP_WARN("sockopt(CHANGE_L/R) is deprecated: fix your app\n");
		return 0;
	case DCCP_SOCKOPT_CCID:
	case DCCP_SOCKOPT_RX_CCID:
	case DCCP_SOCKOPT_TX_CCID:
		return dccp_setsockopt_ccid(sk, optname, optval, optlen);
	}

	if (optlen < (int)sizeof(int))
		return -EINVAL;

	if (get_user(val, (int __user *)optval))
		return -EFAULT;

	if (optname == DCCP_SOCKOPT_SERVICE)
		return dccp_setsockopt_service(sk, val, optval, optlen);

	lock_sock(sk);
	switch (optname) {
	case DCCP_SOCKOPT_SERVER_TIMEWAIT:
		if (dp->dccps_role != DCCP_ROLE_SERVER)
			err = -EOPNOTSUPP;
		else
			dp->dccps_server_timewait = (val != 0);
		break;
	case DCCP_SOCKOPT_SEND_CSCOV:
		err = dccp_setsockopt_cscov(sk, val, false);
		break;
	case DCCP_SOCKOPT_RECV_CSCOV:
		err = dccp_setsockopt_cscov(sk, val, true);
		break;
	case DCCP_SOCKOPT_QPOLICY_ID:
		if (sk->sk_state != DCCP_CLOSED)
			err = -EISCONN;
		else if (val < 0 || val >= DCCPQ_POLICY_MAX)
			err = -EINVAL;
		else
			dp->dccps_qpolicy = val;
		break;
	case DCCP_SOCKOPT_QPOLICY_TXQLEN:
		if (val < 0)
			err = -EINVAL;
		else
			dp->dccps_tx_qlen = val;
		break;
	default:
		err = -ENOPROTOOPT;
		break;
	}
	release_sock(sk);

	return err;
}
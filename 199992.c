static int do_dccp_setsockopt(struct sock *sk, int level, int optname,
		char __user *optval, int optlen)
{
	struct dccp_sock *dp = dccp_sk(sk);
	int val, err = 0;

	if (optlen < sizeof(int))
		return -EINVAL;

	if (get_user(val, (int __user *)optval))
		return -EFAULT;

	if (optname == DCCP_SOCKOPT_SERVICE)
		return dccp_setsockopt_service(sk, val, optval, optlen);

	lock_sock(sk);
	switch (optname) {
	case DCCP_SOCKOPT_PACKET_SIZE:
		DCCP_WARN("sockopt(PACKET_SIZE) is deprecated: fix your app\n");
		err = 0;
		break;
	case DCCP_SOCKOPT_CHANGE_L:
		if (optlen != sizeof(struct dccp_so_feat))
			err = -EINVAL;
		else
			err = dccp_setsockopt_change(sk, DCCPO_CHANGE_L,
						     (struct dccp_so_feat __user *)
						     optval);
		break;
	case DCCP_SOCKOPT_CHANGE_R:
		if (optlen != sizeof(struct dccp_so_feat))
			err = -EINVAL;
		else
			err = dccp_setsockopt_change(sk, DCCPO_CHANGE_R,
						     (struct dccp_so_feat __user *)
						     optval);
		break;
	case DCCP_SOCKOPT_SEND_CSCOV:	/* sender side, RFC 4340, sec. 9.2 */
		if (val < 0 || val > 15)
			err = -EINVAL;
		else
			dp->dccps_pcslen = val;
		break;
	case DCCP_SOCKOPT_RECV_CSCOV:	/* receiver side, RFC 4340 sec. 9.2.1 */
		if (val < 0 || val > 15)
			err = -EINVAL;
		else {
			dp->dccps_pcrlen = val;
			/* FIXME: add feature negotiation,
			 * ChangeL(MinimumChecksumCoverage, val) */
		}
		break;
	default:
		err = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);
	return err;
}
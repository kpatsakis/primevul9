static int do_dccp_getsockopt(struct sock *sk, int level, int optname,
		    char __user *optval, int __user *optlen)
{
	struct dccp_sock *dp;
	int val, len;

	if (get_user(len, optlen))
		return -EFAULT;

	if (len < (int)sizeof(int))
		return -EINVAL;

	dp = dccp_sk(sk);

	switch (optname) {
	case DCCP_SOCKOPT_PACKET_SIZE:
		DCCP_WARN("sockopt(PACKET_SIZE) is deprecated: fix your app\n");
		return 0;
	case DCCP_SOCKOPT_SERVICE:
		return dccp_getsockopt_service(sk, len,
					       (__be32 __user *)optval, optlen);
	case DCCP_SOCKOPT_GET_CUR_MPS:
		val = dp->dccps_mss_cache;
		break;
	case DCCP_SOCKOPT_AVAILABLE_CCIDS:
		return ccid_getsockopt_builtin_ccids(sk, len, optval, optlen);
	case DCCP_SOCKOPT_TX_CCID:
		val = ccid_get_current_tx_ccid(dp);
		if (val < 0)
			return -ENOPROTOOPT;
		break;
	case DCCP_SOCKOPT_RX_CCID:
		val = ccid_get_current_rx_ccid(dp);
		if (val < 0)
			return -ENOPROTOOPT;
		break;
	case DCCP_SOCKOPT_SERVER_TIMEWAIT:
		val = dp->dccps_server_timewait;
		break;
	case DCCP_SOCKOPT_SEND_CSCOV:
		val = dp->dccps_pcslen;
		break;
	case DCCP_SOCKOPT_RECV_CSCOV:
		val = dp->dccps_pcrlen;
		break;
	case DCCP_SOCKOPT_QPOLICY_ID:
		val = dp->dccps_qpolicy;
		break;
	case DCCP_SOCKOPT_QPOLICY_TXQLEN:
		val = dp->dccps_tx_qlen;
		break;
	case 128 ... 191:
		return ccid_hc_rx_getsockopt(dp->dccps_hc_rx_ccid, sk, optname,
					     len, (u32 __user *)optval, optlen);
	case 192 ... 255:
		return ccid_hc_tx_getsockopt(dp->dccps_hc_tx_ccid, sk, optname,
					     len, (u32 __user *)optval, optlen);
	default:
		return -ENOPROTOOPT;
	}

	len = sizeof(val);
	if (put_user(len, optlen) || copy_to_user(optval, &val, len))
		return -EFAULT;

	return 0;
}
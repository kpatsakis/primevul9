static int rds_setsockopt(struct socket *sock, int level, int optname,
			  char __user *optval, unsigned int optlen)
{
	struct rds_sock *rs = rds_sk_to_rs(sock->sk);
	int ret;

	if (level != SOL_RDS) {
		ret = -ENOPROTOOPT;
		goto out;
	}

	switch (optname) {
	case RDS_CANCEL_SENT_TO:
		ret = rds_cancel_sent_to(rs, optval, optlen);
		break;
	case RDS_GET_MR:
		ret = rds_get_mr(rs, optval, optlen);
		break;
	case RDS_GET_MR_FOR_DEST:
		ret = rds_get_mr_for_dest(rs, optval, optlen);
		break;
	case RDS_FREE_MR:
		ret = rds_free_mr(rs, optval, optlen);
		break;
	case RDS_RECVERR:
		ret = rds_set_bool_option(&rs->rs_recverr, optval, optlen);
		break;
	case RDS_CONG_MONITOR:
		ret = rds_cong_monitor(rs, optval, optlen);
		break;
	case SO_RDS_TRANSPORT:
		lock_sock(sock->sk);
		ret = rds_set_transport(rs, optval, optlen);
		release_sock(sock->sk);
		break;
	case SO_TIMESTAMP:
		lock_sock(sock->sk);
		ret = rds_enable_recvtstamp(sock->sk, optval, optlen);
		release_sock(sock->sk);
		break;
	case SO_RDS_MSG_RXPATH_LATENCY:
		ret = rds_recv_track_latency(rs, optval, optlen);
		break;
	default:
		ret = -ENOPROTOOPT;
	}
out:
	return ret;
}
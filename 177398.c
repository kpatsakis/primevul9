static int rds_getsockopt(struct socket *sock, int level, int optname,
			  char __user *optval, int __user *optlen)
{
	struct rds_sock *rs = rds_sk_to_rs(sock->sk);
	int ret = -ENOPROTOOPT, len;
	int trans;

	if (level != SOL_RDS)
		goto out;

	if (get_user(len, optlen)) {
		ret = -EFAULT;
		goto out;
	}

	switch (optname) {
	case RDS_INFO_FIRST ... RDS_INFO_LAST:
		ret = rds_info_getsockopt(sock, optname, optval,
					  optlen);
		break;

	case RDS_RECVERR:
		if (len < sizeof(int))
			ret = -EINVAL;
		else
		if (put_user(rs->rs_recverr, (int __user *) optval) ||
		    put_user(sizeof(int), optlen))
			ret = -EFAULT;
		else
			ret = 0;
		break;
	case SO_RDS_TRANSPORT:
		if (len < sizeof(int)) {
			ret = -EINVAL;
			break;
		}
		trans = (rs->rs_transport ? rs->rs_transport->t_type :
			 RDS_TRANS_NONE); /* unbound */
		if (put_user(trans, (int __user *)optval) ||
		    put_user(sizeof(int), optlen))
			ret = -EFAULT;
		else
			ret = 0;
		break;
	default:
		break;
	}

out:
	return ret;

}
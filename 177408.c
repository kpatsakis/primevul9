static int rds_set_transport(struct rds_sock *rs, char __user *optval,
			     int optlen)
{
	int t_type;

	if (rs->rs_transport)
		return -EOPNOTSUPP; /* previously attached to transport */

	if (optlen != sizeof(int))
		return -EINVAL;

	if (copy_from_user(&t_type, (int __user *)optval, sizeof(t_type)))
		return -EFAULT;

	if (t_type < 0 || t_type >= RDS_TRANS_COUNT)
		return -EINVAL;

	rs->rs_transport = rds_trans_get(t_type);

	return rs->rs_transport ? 0 : -ENOPROTOOPT;
}
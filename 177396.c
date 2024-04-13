static int rds_enable_recvtstamp(struct sock *sk, char __user *optval,
				 int optlen)
{
	int val, valbool;

	if (optlen != sizeof(int))
		return -EFAULT;

	if (get_user(val, (int __user *)optval))
		return -EFAULT;

	valbool = val ? 1 : 0;

	if (valbool)
		sock_set_flag(sk, SOCK_RCVTSTAMP);
	else
		sock_reset_flag(sk, SOCK_RCVTSTAMP);

	return 0;
}
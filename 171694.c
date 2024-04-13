int sock_get_timestampns(struct sock *sk, struct timespec __user *userstamp)
{
	struct timespec ts;
	if (!sock_flag(sk, SOCK_TIMESTAMP))
		sock_enable_timestamp(sk, SOCK_TIMESTAMP);
	ts = ktime_to_timespec(sk->sk_stamp);
	if (ts.tv_sec == -1)
		return -ENOENT;
	if (ts.tv_sec == 0) {
		sk->sk_stamp = ktime_get_real();
		ts = ktime_to_timespec(sk->sk_stamp);
	}
	return copy_to_user(userstamp, &ts, sizeof(ts)) ? -EFAULT : 0;
}
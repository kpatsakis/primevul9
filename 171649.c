int sock_get_timestamp(struct sock *sk, struct timeval __user *userstamp)
{
	struct timeval tv;
	if (!sock_flag(sk, SOCK_TIMESTAMP))
		sock_enable_timestamp(sk, SOCK_TIMESTAMP);
	tv = ktime_to_timeval(sk->sk_stamp);
	if (tv.tv_sec == -1)
		return -ENOENT;
	if (tv.tv_sec == 0) {
		sk->sk_stamp = ktime_get_real();
		tv = ktime_to_timeval(sk->sk_stamp);
	}
	return copy_to_user(userstamp, &tv, sizeof(tv)) ? -EFAULT : 0;
}
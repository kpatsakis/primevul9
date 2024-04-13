netlink_kernel_release(struct sock *sk)
{
	if (sk == NULL || sk->sk_socket == NULL)
		return;

	sock_release(sk->sk_socket);
}
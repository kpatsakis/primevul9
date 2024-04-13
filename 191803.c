static inline int netlink_is_kernel(struct sock *sk)
{
	return nlk_sk(sk)->flags & NETLINK_F_KERNEL_SOCKET;
}
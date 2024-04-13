int compat_dccp_getsockopt(struct sock *sk, int level, int optname,
			   char __user *optval, int __user *optlen)
{
	if (level != SOL_DCCP)
		return inet_csk_compat_getsockopt(sk, level, optname,
						  optval, optlen);
	return do_dccp_getsockopt(sk, level, optname, optval, optlen);
}
int compat_dccp_setsockopt(struct sock *sk, int level, int optname,
			   char __user *optval, unsigned int optlen)
{
	if (level != SOL_DCCP)
		return inet_csk_compat_setsockopt(sk, level, optname,
						  optval, optlen);
	return do_dccp_setsockopt(sk, level, optname, optval, optlen);
}
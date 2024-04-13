int dccp_setsockopt(struct sock *sk, int level, int optname,
		    char __user *optval, int optlen)
{
	if (level != SOL_DCCP)
		return inet_csk(sk)->icsk_af_ops->setsockopt(sk, level,
							     optname, optval,
							     optlen);
	return do_dccp_setsockopt(sk, level, optname, optval, optlen);
}
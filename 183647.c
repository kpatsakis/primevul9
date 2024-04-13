int ip_mc_join_group_ssm(struct sock *sk, struct ip_mreqn *imr,
			 unsigned int mode)
{
	return __ip_mc_join_group(sk, imr, mode);
}
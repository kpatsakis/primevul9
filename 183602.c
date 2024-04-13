int ip_mc_join_group(struct sock *sk, struct ip_mreqn *imr)
{
	return __ip_mc_join_group(sk, imr, MCAST_EXCLUDE);
}
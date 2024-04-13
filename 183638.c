int ip_mc_sf_allow(struct sock *sk, __be32 loc_addr, __be32 rmt_addr,
		   int dif, int sdif)
{
	struct inet_sock *inet = inet_sk(sk);
	struct ip_mc_socklist *pmc;
	struct ip_sf_socklist *psl;
	int i;
	int ret;

	ret = 1;
	if (!ipv4_is_multicast(loc_addr))
		goto out;

	rcu_read_lock();
	for_each_pmc_rcu(inet, pmc) {
		if (pmc->multi.imr_multiaddr.s_addr == loc_addr &&
		    (pmc->multi.imr_ifindex == dif ||
		     (sdif && pmc->multi.imr_ifindex == sdif)))
			break;
	}
	ret = inet->mc_all;
	if (!pmc)
		goto unlock;
	psl = rcu_dereference(pmc->sflist);
	ret = (pmc->sfmode == MCAST_EXCLUDE);
	if (!psl)
		goto unlock;

	for (i = 0; i < psl->sl_count; i++) {
		if (psl->sl_addr[i] == rmt_addr)
			break;
	}
	ret = 0;
	if (pmc->sfmode == MCAST_INCLUDE && i >= psl->sl_count)
		goto unlock;
	if (pmc->sfmode == MCAST_EXCLUDE && i < psl->sl_count)
		goto unlock;
	ret = 1;
unlock:
	rcu_read_unlock();
out:
	return ret;
}
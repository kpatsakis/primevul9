static void ip_mc_clear_src(struct ip_mc_list *pmc)
{
	struct ip_sf_list *tomb, *sources;

	spin_lock_bh(&pmc->lock);
	tomb = pmc->tomb;
	pmc->tomb = NULL;
	sources = pmc->sources;
	pmc->sources = NULL;
	pmc->sfmode = MCAST_EXCLUDE;
	pmc->sfcount[MCAST_INCLUDE] = 0;
	pmc->sfcount[MCAST_EXCLUDE] = 1;
	spin_unlock_bh(&pmc->lock);

	ip_sf_list_clear_all(tomb);
	ip_sf_list_clear_all(sources);
}
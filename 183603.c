static int grec_size(struct ip_mc_list *pmc, int type, int gdel, int sdel)
{
	return sizeof(struct igmpv3_grec) + 4*igmp_scount(pmc, type, gdel, sdel);
}
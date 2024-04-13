static void kfree_pmc(struct ip_mc_list *pmc)
{
	ip_sf_list_clear_all(pmc->sources);
	ip_sf_list_clear_all(pmc->tomb);
	kfree(pmc);
}
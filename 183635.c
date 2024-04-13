static void igmpv3_clear_delrec(struct in_device *in_dev)
{
	struct ip_mc_list *pmc, *nextpmc;

	spin_lock_bh(&in_dev->mc_tomb_lock);
	pmc = in_dev->mc_tomb;
	in_dev->mc_tomb = NULL;
	spin_unlock_bh(&in_dev->mc_tomb_lock);

	for (; pmc; pmc = nextpmc) {
		nextpmc = pmc->next;
		ip_mc_clear_src(pmc);
		in_dev_put(pmc->interface);
		kfree_pmc(pmc);
	}
	/* clear dead sources, too */
	rcu_read_lock();
	for_each_pmc_rcu(in_dev, pmc) {
		struct ip_sf_list *psf;

		spin_lock_bh(&pmc->lock);
		psf = pmc->tomb;
		pmc->tomb = NULL;
		spin_unlock_bh(&pmc->lock);
		ip_sf_list_clear_all(psf);
	}
	rcu_read_unlock();
}
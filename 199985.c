static int __init dccp_init(void)
{
	unsigned long goal;
	int ehash_order, bhash_order, i;
	int rc = -ENOBUFS;

	dccp_hashinfo.bind_bucket_cachep =
		kmem_cache_create("dccp_bind_bucket",
				  sizeof(struct inet_bind_bucket), 0,
				  SLAB_HWCACHE_ALIGN, NULL, NULL);
	if (!dccp_hashinfo.bind_bucket_cachep)
		goto out;

	/*
	 * Size and allocate the main established and bind bucket
	 * hash tables.
	 *
	 * The methodology is similar to that of the buffer cache.
	 */
	if (num_physpages >= (128 * 1024))
		goal = num_physpages >> (21 - PAGE_SHIFT);
	else
		goal = num_physpages >> (23 - PAGE_SHIFT);

	if (thash_entries)
		goal = (thash_entries *
			sizeof(struct inet_ehash_bucket)) >> PAGE_SHIFT;
	for (ehash_order = 0; (1UL << ehash_order) < goal; ehash_order++)
		;
	do {
		dccp_hashinfo.ehash_size = (1UL << ehash_order) * PAGE_SIZE /
					sizeof(struct inet_ehash_bucket);
		while (dccp_hashinfo.ehash_size &
		       (dccp_hashinfo.ehash_size - 1))
			dccp_hashinfo.ehash_size--;
		dccp_hashinfo.ehash = (struct inet_ehash_bucket *)
			__get_free_pages(GFP_ATOMIC, ehash_order);
	} while (!dccp_hashinfo.ehash && --ehash_order > 0);

	if (!dccp_hashinfo.ehash) {
		DCCP_CRIT("Failed to allocate DCCP established hash table");
		goto out_free_bind_bucket_cachep;
	}

	for (i = 0; i < dccp_hashinfo.ehash_size; i++) {
		rwlock_init(&dccp_hashinfo.ehash[i].lock);
		INIT_HLIST_HEAD(&dccp_hashinfo.ehash[i].chain);
		INIT_HLIST_HEAD(&dccp_hashinfo.ehash[i].twchain);
	}

	bhash_order = ehash_order;

	do {
		dccp_hashinfo.bhash_size = (1UL << bhash_order) * PAGE_SIZE /
					sizeof(struct inet_bind_hashbucket);
		if ((dccp_hashinfo.bhash_size > (64 * 1024)) &&
		    bhash_order > 0)
			continue;
		dccp_hashinfo.bhash = (struct inet_bind_hashbucket *)
			__get_free_pages(GFP_ATOMIC, bhash_order);
	} while (!dccp_hashinfo.bhash && --bhash_order >= 0);

	if (!dccp_hashinfo.bhash) {
		DCCP_CRIT("Failed to allocate DCCP bind hash table");
		goto out_free_dccp_ehash;
	}

	for (i = 0; i < dccp_hashinfo.bhash_size; i++) {
		spin_lock_init(&dccp_hashinfo.bhash[i].lock);
		INIT_HLIST_HEAD(&dccp_hashinfo.bhash[i].chain);
	}

	rc = dccp_mib_init();
	if (rc)
		goto out_free_dccp_bhash;

	rc = dccp_ackvec_init();
	if (rc)
		goto out_free_dccp_mib;

	rc = dccp_sysctl_init();
	if (rc)
		goto out_ackvec_exit;
out:
	return rc;
out_ackvec_exit:
	dccp_ackvec_exit();
out_free_dccp_mib:
	dccp_mib_exit();
out_free_dccp_bhash:
	free_pages((unsigned long)dccp_hashinfo.bhash, bhash_order);
	dccp_hashinfo.bhash = NULL;
out_free_dccp_ehash:
	free_pages((unsigned long)dccp_hashinfo.ehash, ehash_order);
	dccp_hashinfo.ehash = NULL;
out_free_bind_bucket_cachep:
	kmem_cache_destroy(dccp_hashinfo.bind_bucket_cachep);
	dccp_hashinfo.bind_bucket_cachep = NULL;
	goto out;
}
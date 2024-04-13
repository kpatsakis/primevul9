static int __init dccp_init(void)
{
	unsigned long goal;
	int ehash_order, bhash_order, i;
	int rc;

	BUILD_BUG_ON(sizeof(struct dccp_skb_cb) >
		     FIELD_SIZEOF(struct sk_buff, cb));
	rc = percpu_counter_init(&dccp_orphan_count, 0, GFP_KERNEL);
	if (rc)
		goto out_fail;
	rc = -ENOBUFS;
	inet_hashinfo_init(&dccp_hashinfo);
	dccp_hashinfo.bind_bucket_cachep =
		kmem_cache_create("dccp_bind_bucket",
				  sizeof(struct inet_bind_bucket), 0,
				  SLAB_HWCACHE_ALIGN, NULL);
	if (!dccp_hashinfo.bind_bucket_cachep)
		goto out_free_percpu;

	/*
	 * Size and allocate the main established and bind bucket
	 * hash tables.
	 *
	 * The methodology is similar to that of the buffer cache.
	 */
	if (totalram_pages >= (128 * 1024))
		goal = totalram_pages >> (21 - PAGE_SHIFT);
	else
		goal = totalram_pages >> (23 - PAGE_SHIFT);

	if (thash_entries)
		goal = (thash_entries *
			sizeof(struct inet_ehash_bucket)) >> PAGE_SHIFT;
	for (ehash_order = 0; (1UL << ehash_order) < goal; ehash_order++)
		;
	do {
		unsigned long hash_size = (1UL << ehash_order) * PAGE_SIZE /
					sizeof(struct inet_ehash_bucket);

		while (hash_size & (hash_size - 1))
			hash_size--;
		dccp_hashinfo.ehash_mask = hash_size - 1;
		dccp_hashinfo.ehash = (struct inet_ehash_bucket *)
			__get_free_pages(GFP_ATOMIC|__GFP_NOWARN, ehash_order);
	} while (!dccp_hashinfo.ehash && --ehash_order > 0);

	if (!dccp_hashinfo.ehash) {
		DCCP_CRIT("Failed to allocate DCCP established hash table");
		goto out_free_bind_bucket_cachep;
	}

	for (i = 0; i <= dccp_hashinfo.ehash_mask; i++)
		INIT_HLIST_NULLS_HEAD(&dccp_hashinfo.ehash[i].chain, i);

	if (inet_ehash_locks_alloc(&dccp_hashinfo))
			goto out_free_dccp_ehash;

	bhash_order = ehash_order;

	do {
		dccp_hashinfo.bhash_size = (1UL << bhash_order) * PAGE_SIZE /
					sizeof(struct inet_bind_hashbucket);
		if ((dccp_hashinfo.bhash_size > (64 * 1024)) &&
		    bhash_order > 0)
			continue;
		dccp_hashinfo.bhash = (struct inet_bind_hashbucket *)
			__get_free_pages(GFP_ATOMIC|__GFP_NOWARN, bhash_order);
	} while (!dccp_hashinfo.bhash && --bhash_order >= 0);

	if (!dccp_hashinfo.bhash) {
		DCCP_CRIT("Failed to allocate DCCP bind hash table");
		goto out_free_dccp_locks;
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

	rc = ccid_initialize_builtins();
	if (rc)
		goto out_sysctl_exit;

	dccp_timestamping_init();

	return 0;

out_sysctl_exit:
	dccp_sysctl_exit();
out_ackvec_exit:
	dccp_ackvec_exit();
out_free_dccp_mib:
	dccp_mib_exit();
out_free_dccp_bhash:
	free_pages((unsigned long)dccp_hashinfo.bhash, bhash_order);
out_free_dccp_locks:
	inet_ehash_locks_free(&dccp_hashinfo);
out_free_dccp_ehash:
	free_pages((unsigned long)dccp_hashinfo.ehash, ehash_order);
out_free_bind_bucket_cachep:
	kmem_cache_destroy(dccp_hashinfo.bind_bucket_cachep);
out_free_percpu:
	percpu_counter_destroy(&dccp_orphan_count);
out_fail:
	dccp_hashinfo.bhash = NULL;
	dccp_hashinfo.ehash = NULL;
	dccp_hashinfo.bind_bucket_cachep = NULL;
	return rc;
}
static void __exit dccp_fini(void)
{
	ccid_cleanup_builtins();
	dccp_mib_exit();
	free_pages((unsigned long)dccp_hashinfo.bhash,
		   get_order(dccp_hashinfo.bhash_size *
			     sizeof(struct inet_bind_hashbucket)));
	free_pages((unsigned long)dccp_hashinfo.ehash,
		   get_order((dccp_hashinfo.ehash_mask + 1) *
			     sizeof(struct inet_ehash_bucket)));
	inet_ehash_locks_free(&dccp_hashinfo);
	kmem_cache_destroy(dccp_hashinfo.bind_bucket_cachep);
	dccp_ackvec_exit();
	dccp_sysctl_exit();
	percpu_counter_destroy(&dccp_orphan_count);
}
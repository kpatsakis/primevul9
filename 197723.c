static void cgw_remove_all_jobs(struct net *net)
{
	struct cgw_job *gwj = NULL;
	struct hlist_node *nx;

	ASSERT_RTNL();

	hlist_for_each_entry_safe(gwj, nx, &net->can.cgw_list, list) {
		hlist_del(&gwj->list);
		cgw_unregister_filter(net, gwj);
		kmem_cache_free(cgw_cache, gwj);
	}
}
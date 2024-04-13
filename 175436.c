static void htab_map_free(struct bpf_map *map)
{
	struct bpf_htab *htab = container_of(map, struct bpf_htab, map);
	int i;

	/* bpf_free_used_maps() or close(map_fd) will trigger this map_free callback.
	 * bpf_free_used_maps() is called after bpf prog is no longer executing.
	 * There is no need to synchronize_rcu() here to protect map elements.
	 */

	/* some of free_htab_elem() callbacks for elements of this map may
	 * not have executed. Wait for them.
	 */
	rcu_barrier();
	if (!htab_is_prealloc(htab))
		delete_all_elements(htab);
	else
		prealloc_destroy(htab);

	free_percpu(htab->extra_elems);
	bpf_map_area_free(htab->buckets);
	for (i = 0; i < HASHTAB_MAP_LOCK_COUNT; i++)
		free_percpu(htab->map_locked[i]);
	lockdep_unregister_key(&htab->lockdep_key);
	kfree(htab);
}
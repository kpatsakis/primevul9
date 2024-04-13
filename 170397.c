static void __bpf_map_put(struct bpf_map *map, bool do_idr_lock)
{
	if (atomic64_dec_and_test(&map->refcnt)) {
		/* bpf_map_free_id() must be called first */
		bpf_map_free_id(map, do_idr_lock);
		btf_put(map->btf);
		INIT_WORK(&map->work, bpf_map_free_deferred);
		schedule_work(&map->work);
	}
}
static void bpf_map_free_deferred(struct work_struct *work)
{
	struct bpf_map *map = container_of(work, struct bpf_map, work);

	security_bpf_map_free(map);
	bpf_map_release_memcg(map);
	/* implementation dependent freeing */
	map->ops->map_free(map);
}
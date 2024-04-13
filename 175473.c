int bpf_percpu_hash_update(struct bpf_map *map, void *key, void *value,
			   u64 map_flags)
{
	struct bpf_htab *htab = container_of(map, struct bpf_htab, map);
	int ret;

	rcu_read_lock();
	if (htab_is_lru(htab))
		ret = __htab_lru_percpu_map_update_elem(map, key, value,
							map_flags, true);
	else
		ret = __htab_percpu_map_update_elem(map, key, value, map_flags,
						    true);
	rcu_read_unlock();

	return ret;
}
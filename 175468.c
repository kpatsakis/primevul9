int bpf_percpu_hash_copy(struct bpf_map *map, void *key, void *value)
{
	struct htab_elem *l;
	void __percpu *pptr;
	int ret = -ENOENT;
	int cpu, off = 0;
	u32 size;

	/* per_cpu areas are zero-filled and bpf programs can only
	 * access 'value_size' of them, so copying rounded areas
	 * will not leak any kernel data
	 */
	size = round_up(map->value_size, 8);
	rcu_read_lock();
	l = __htab_map_lookup_elem(map, key);
	if (!l)
		goto out;
	/* We do not mark LRU map element here in order to not mess up
	 * eviction heuristics when user space does a map walk.
	 */
	pptr = htab_elem_get_ptr(l, map->key_size);
	for_each_possible_cpu(cpu) {
		bpf_long_memcpy(value + off,
				per_cpu_ptr(pptr, cpu), size);
		off += size;
	}
	ret = 0;
out:
	rcu_read_unlock();
	return ret;
}
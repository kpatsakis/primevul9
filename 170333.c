static int bpf_map_copy_value(struct bpf_map *map, void *key, void *value,
			      __u64 flags)
{
	void *ptr;
	int err;

	if (bpf_map_is_dev_bound(map))
		return bpf_map_offload_lookup_elem(map, key, value);

	bpf_disable_instrumentation();
	if (map->map_type == BPF_MAP_TYPE_PERCPU_HASH ||
	    map->map_type == BPF_MAP_TYPE_LRU_PERCPU_HASH) {
		err = bpf_percpu_hash_copy(map, key, value);
	} else if (map->map_type == BPF_MAP_TYPE_PERCPU_ARRAY) {
		err = bpf_percpu_array_copy(map, key, value);
	} else if (map->map_type == BPF_MAP_TYPE_PERCPU_CGROUP_STORAGE) {
		err = bpf_percpu_cgroup_storage_copy(map, key, value);
	} else if (map->map_type == BPF_MAP_TYPE_STACK_TRACE) {
		err = bpf_stackmap_copy(map, key, value);
	} else if (IS_FD_ARRAY(map) || IS_FD_PROG_ARRAY(map)) {
		err = bpf_fd_array_map_lookup_elem(map, key, value);
	} else if (IS_FD_HASH(map)) {
		err = bpf_fd_htab_map_lookup_elem(map, key, value);
	} else if (map->map_type == BPF_MAP_TYPE_REUSEPORT_SOCKARRAY) {
		err = bpf_fd_reuseport_array_lookup_elem(map, key, value);
	} else if (map->map_type == BPF_MAP_TYPE_QUEUE ||
		   map->map_type == BPF_MAP_TYPE_STACK) {
		err = map->ops->map_peek_elem(map, value);
	} else if (map->map_type == BPF_MAP_TYPE_STRUCT_OPS) {
		/* struct_ops map requires directly updating "value" */
		err = bpf_struct_ops_map_sys_lookup_elem(map, key, value);
	} else {
		rcu_read_lock();
		if (map->ops->map_lookup_elem_sys_only)
			ptr = map->ops->map_lookup_elem_sys_only(map, key);
		else
			ptr = map->ops->map_lookup_elem(map, key);
		if (IS_ERR(ptr)) {
			err = PTR_ERR(ptr);
		} else if (!ptr) {
			err = -ENOENT;
		} else {
			err = 0;
			if (flags & BPF_F_LOCK)
				/* lock 'ptr' and copy everything but lock */
				copy_map_value_locked(map, value, ptr, true);
			else
				copy_map_value(map, value, ptr);
			/* mask lock, since value wasn't zero inited */
			check_and_init_map_lock(map, value);
		}
		rcu_read_unlock();
	}

	bpf_enable_instrumentation();
	maybe_wait_bpf_programs(map);

	return err;
}
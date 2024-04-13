static int bpf_map_update_value(struct bpf_map *map, struct fd f, void *key,
				void *value, __u64 flags)
{
	int err;

	/* Need to create a kthread, thus must support schedule */
	if (bpf_map_is_dev_bound(map)) {
		return bpf_map_offload_update_elem(map, key, value, flags);
	} else if (map->map_type == BPF_MAP_TYPE_CPUMAP ||
		   map->map_type == BPF_MAP_TYPE_STRUCT_OPS) {
		return map->ops->map_update_elem(map, key, value, flags);
	} else if (map->map_type == BPF_MAP_TYPE_SOCKHASH ||
		   map->map_type == BPF_MAP_TYPE_SOCKMAP) {
		return sock_map_update_elem_sys(map, key, value, flags);
	} else if (IS_FD_PROG_ARRAY(map)) {
		return bpf_fd_array_map_update_elem(map, f.file, key, value,
						    flags);
	}

	bpf_disable_instrumentation();
	if (map->map_type == BPF_MAP_TYPE_PERCPU_HASH ||
	    map->map_type == BPF_MAP_TYPE_LRU_PERCPU_HASH) {
		err = bpf_percpu_hash_update(map, key, value, flags);
	} else if (map->map_type == BPF_MAP_TYPE_PERCPU_ARRAY) {
		err = bpf_percpu_array_update(map, key, value, flags);
	} else if (map->map_type == BPF_MAP_TYPE_PERCPU_CGROUP_STORAGE) {
		err = bpf_percpu_cgroup_storage_update(map, key, value,
						       flags);
	} else if (IS_FD_ARRAY(map)) {
		rcu_read_lock();
		err = bpf_fd_array_map_update_elem(map, f.file, key, value,
						   flags);
		rcu_read_unlock();
	} else if (map->map_type == BPF_MAP_TYPE_HASH_OF_MAPS) {
		rcu_read_lock();
		err = bpf_fd_htab_map_update_elem(map, f.file, key, value,
						  flags);
		rcu_read_unlock();
	} else if (map->map_type == BPF_MAP_TYPE_REUSEPORT_SOCKARRAY) {
		/* rcu_read_lock() is not needed */
		err = bpf_fd_reuseport_array_update_elem(map, key, value,
							 flags);
	} else if (map->map_type == BPF_MAP_TYPE_QUEUE ||
		   map->map_type == BPF_MAP_TYPE_STACK) {
		err = map->ops->map_push_elem(map, value, flags);
	} else {
		rcu_read_lock();
		err = map->ops->map_update_elem(map, key, value, flags);
		rcu_read_unlock();
	}
	bpf_enable_instrumentation();
	maybe_wait_bpf_programs(map);

	return err;
}
int generic_map_delete_batch(struct bpf_map *map,
			     const union bpf_attr *attr,
			     union bpf_attr __user *uattr)
{
	void __user *keys = u64_to_user_ptr(attr->batch.keys);
	u32 cp, max_count;
	int err = 0;
	void *key;

	if (attr->batch.elem_flags & ~BPF_F_LOCK)
		return -EINVAL;

	if ((attr->batch.elem_flags & BPF_F_LOCK) &&
	    !map_value_has_spin_lock(map)) {
		return -EINVAL;
	}

	max_count = attr->batch.count;
	if (!max_count)
		return 0;

	key = kmalloc(map->key_size, GFP_USER | __GFP_NOWARN);
	if (!key)
		return -ENOMEM;

	for (cp = 0; cp < max_count; cp++) {
		err = -EFAULT;
		if (copy_from_user(key, keys + cp * map->key_size,
				   map->key_size))
			break;

		if (bpf_map_is_dev_bound(map)) {
			err = bpf_map_offload_delete_elem(map, key);
			break;
		}

		bpf_disable_instrumentation();
		rcu_read_lock();
		err = map->ops->map_delete_elem(map, key);
		rcu_read_unlock();
		bpf_enable_instrumentation();
		maybe_wait_bpf_programs(map);
		if (err)
			break;
	}
	if (copy_to_user(&uattr->batch.count, &cp, sizeof(cp)))
		err = -EFAULT;

	kfree(key);
	return err;
}
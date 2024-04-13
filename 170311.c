int generic_map_update_batch(struct bpf_map *map,
			     const union bpf_attr *attr,
			     union bpf_attr __user *uattr)
{
	void __user *values = u64_to_user_ptr(attr->batch.values);
	void __user *keys = u64_to_user_ptr(attr->batch.keys);
	u32 value_size, cp, max_count;
	int ufd = attr->map_fd;
	void *key, *value;
	struct fd f;
	int err = 0;

	f = fdget(ufd);
	if (attr->batch.elem_flags & ~BPF_F_LOCK)
		return -EINVAL;

	if ((attr->batch.elem_flags & BPF_F_LOCK) &&
	    !map_value_has_spin_lock(map)) {
		return -EINVAL;
	}

	value_size = bpf_map_value_size(map);

	max_count = attr->batch.count;
	if (!max_count)
		return 0;

	key = kmalloc(map->key_size, GFP_USER | __GFP_NOWARN);
	if (!key)
		return -ENOMEM;

	value = kmalloc(value_size, GFP_USER | __GFP_NOWARN);
	if (!value) {
		kfree(key);
		return -ENOMEM;
	}

	for (cp = 0; cp < max_count; cp++) {
		err = -EFAULT;
		if (copy_from_user(key, keys + cp * map->key_size,
		    map->key_size) ||
		    copy_from_user(value, values + cp * value_size, value_size))
			break;

		err = bpf_map_update_value(map, f, key, value,
					   attr->batch.elem_flags);

		if (err)
			break;
	}

	if (copy_to_user(&uattr->batch.count, &cp, sizeof(cp)))
		err = -EFAULT;

	kfree(value);
	kfree(key);
	return err;
}
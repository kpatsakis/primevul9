int generic_map_lookup_batch(struct bpf_map *map,
				    const union bpf_attr *attr,
				    union bpf_attr __user *uattr)
{
	void __user *uobatch = u64_to_user_ptr(attr->batch.out_batch);
	void __user *ubatch = u64_to_user_ptr(attr->batch.in_batch);
	void __user *values = u64_to_user_ptr(attr->batch.values);
	void __user *keys = u64_to_user_ptr(attr->batch.keys);
	void *buf, *buf_prevkey, *prev_key, *key, *value;
	int err, retry = MAP_LOOKUP_RETRIES;
	u32 value_size, cp, max_count;

	if (attr->batch.elem_flags & ~BPF_F_LOCK)
		return -EINVAL;

	if ((attr->batch.elem_flags & BPF_F_LOCK) &&
	    !map_value_has_spin_lock(map))
		return -EINVAL;

	value_size = bpf_map_value_size(map);

	max_count = attr->batch.count;
	if (!max_count)
		return 0;

	if (put_user(0, &uattr->batch.count))
		return -EFAULT;

	buf_prevkey = kmalloc(map->key_size, GFP_USER | __GFP_NOWARN);
	if (!buf_prevkey)
		return -ENOMEM;

	buf = kmalloc(map->key_size + value_size, GFP_USER | __GFP_NOWARN);
	if (!buf) {
		kfree(buf_prevkey);
		return -ENOMEM;
	}

	err = -EFAULT;
	prev_key = NULL;
	if (ubatch && copy_from_user(buf_prevkey, ubatch, map->key_size))
		goto free_buf;
	key = buf;
	value = key + map->key_size;
	if (ubatch)
		prev_key = buf_prevkey;

	for (cp = 0; cp < max_count;) {
		rcu_read_lock();
		err = map->ops->map_get_next_key(map, prev_key, key);
		rcu_read_unlock();
		if (err)
			break;
		err = bpf_map_copy_value(map, key, value,
					 attr->batch.elem_flags);

		if (err == -ENOENT) {
			if (retry) {
				retry--;
				continue;
			}
			err = -EINTR;
			break;
		}

		if (err)
			goto free_buf;

		if (copy_to_user(keys + cp * map->key_size, key,
				 map->key_size)) {
			err = -EFAULT;
			goto free_buf;
		}
		if (copy_to_user(values + cp * value_size, value, value_size)) {
			err = -EFAULT;
			goto free_buf;
		}

		if (!prev_key)
			prev_key = buf_prevkey;

		swap(prev_key, key);
		retry = MAP_LOOKUP_RETRIES;
		cp++;
	}

	if (err == -EFAULT)
		goto free_buf;

	if ((copy_to_user(&uattr->batch.count, &cp, sizeof(cp)) ||
		    (cp && copy_to_user(uobatch, prev_key, map->key_size))))
		err = -EFAULT;

free_buf:
	kfree(buf_prevkey);
	kfree(buf);
	return err;
}
static int map_get_next_key(union bpf_attr *attr)
{
	void __user *ukey = u64_to_user_ptr(attr->key);
	void __user *unext_key = u64_to_user_ptr(attr->next_key);
	int ufd = attr->map_fd;
	struct bpf_map *map;
	void *key, *next_key;
	struct fd f;
	int err;

	if (CHECK_ATTR(BPF_MAP_GET_NEXT_KEY))
		return -EINVAL;

	f = fdget(ufd);
	map = __bpf_map_get(f);
	if (IS_ERR(map))
		return PTR_ERR(map);
	if (!(map_get_sys_perms(map, f) & FMODE_CAN_READ)) {
		err = -EPERM;
		goto err_put;
	}

	if (ukey) {
		key = __bpf_copy_key(ukey, map->key_size);
		if (IS_ERR(key)) {
			err = PTR_ERR(key);
			goto err_put;
		}
	} else {
		key = NULL;
	}

	err = -ENOMEM;
	next_key = kmalloc(map->key_size, GFP_USER);
	if (!next_key)
		goto free_key;

	if (bpf_map_is_dev_bound(map)) {
		err = bpf_map_offload_get_next_key(map, key, next_key);
		goto out;
	}

	rcu_read_lock();
	err = map->ops->map_get_next_key(map, key, next_key);
	rcu_read_unlock();
out:
	if (err)
		goto free_next_key;

	err = -EFAULT;
	if (copy_to_user(unext_key, next_key, map->key_size) != 0)
		goto free_next_key;

	err = 0;

free_next_key:
	kfree(next_key);
free_key:
	kfree(key);
err_put:
	fdput(f);
	return err;
}
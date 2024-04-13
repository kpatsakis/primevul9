void *bpf_map_fd_get_ptr(struct bpf_map *map,
			 struct file *map_file /* not used */,
			 int ufd)
{
	struct bpf_map *inner_map;
	struct fd f;

	f = fdget(ufd);
	inner_map = __bpf_map_get(f);
	if (IS_ERR(inner_map))
		return inner_map;

	if (bpf_map_meta_equal(map->inner_map_meta, inner_map))
		inner_map = bpf_map_inc(inner_map, false);
	else
		inner_map = ERR_PTR(-EINVAL);

	fdput(f);
	return inner_map;
}
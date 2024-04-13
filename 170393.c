static __poll_t bpf_map_poll(struct file *filp, struct poll_table_struct *pts)
{
	struct bpf_map *map = filp->private_data;

	if (map->ops->map_poll)
		return map->ops->map_poll(map, filp, pts);

	return EPOLLERR;
}
static void xfrm_confirm_neigh(const struct dst_entry *dst, const void *daddr)
{
	const struct dst_entry *path = xfrm_dst_path(dst);

	daddr = xfrm_get_dst_nexthop(dst, daddr);
	path->ops->confirm_neigh(path, daddr);
}
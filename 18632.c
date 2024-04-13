static unsigned int xfrm_mtu(const struct dst_entry *dst)
{
	unsigned int mtu = dst_metric_raw(dst, RTAX_MTU);

	return mtu ? : dst_mtu(xfrm_dst_path(dst));
}
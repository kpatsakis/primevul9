static unsigned int xfrm_default_advmss(const struct dst_entry *dst)
{
	return dst_metric_advmss(xfrm_dst_path(dst));
}
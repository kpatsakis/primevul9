static void xfrm_init_path(struct xfrm_dst *path, struct dst_entry *dst,
			   int nfheader_len)
{
	if (dst->ops->family == AF_INET6) {
		struct rt6_info *rt = (struct rt6_info *)dst;
		path->path_cookie = rt6_get_cookie(rt);
		path->u.rt6.rt6i_nfheader_len = nfheader_len;
	}
}
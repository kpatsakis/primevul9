static int decode_attr_group(struct xdr_stream *xdr, uint32_t *bitmap,
		const struct nfs_server *server, uint32_t *gid,
		struct nfs4_string *group_name)
{
	uint32_t len;
	__be32 *p;
	int ret = 0;

	*gid = -2;
	if (unlikely(bitmap[1] & (FATTR4_WORD1_OWNER_GROUP - 1U)))
		return -EIO;
	if (likely(bitmap[1] & FATTR4_WORD1_OWNER_GROUP)) {
		p = xdr_inline_decode(xdr, 4);
		if (unlikely(!p))
			goto out_overflow;
		len = be32_to_cpup(p);
		p = xdr_inline_decode(xdr, len);
		if (unlikely(!p))
			goto out_overflow;
		if (group_name != NULL) {
			group_name->data = kmemdup(p, len, GFP_NOWAIT);
			if (group_name->data != NULL) {
				group_name->len = len;
				ret = NFS_ATTR_FATTR_GROUP_NAME;
			}
		} else if (len < XDR_MAX_NETOBJ) {
			if (nfs_map_group_to_gid(server, (char *)p, len, gid) == 0)
				ret = NFS_ATTR_FATTR_GROUP;
			else
				dprintk("%s: nfs_map_group_to_gid failed!\n",
						__func__);
		} else
			dprintk("%s: name too long (%u)!\n",
					__func__, len);
		bitmap[1] &= ~FATTR4_WORD1_OWNER_GROUP;
	}
	dprintk("%s: gid=%d\n", __func__, (int)*gid);
	return ret;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}
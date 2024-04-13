static void encode_attrs(struct xdr_stream *xdr, const struct iattr *iap, const struct nfs_server *server)
{
	char owner_name[IDMAP_NAMESZ];
	char owner_group[IDMAP_NAMESZ];
	int owner_namelen = 0;
	int owner_grouplen = 0;
	__be32 *p;
	__be32 *q;
	int len;
	uint32_t bmval0 = 0;
	uint32_t bmval1 = 0;

	/*
	 * We reserve enough space to write the entire attribute buffer at once.
	 * In the worst-case, this would be
	 *   12(bitmap) + 4(attrlen) + 8(size) + 4(mode) + 4(atime) + 4(mtime)
	 *          = 36 bytes, plus any contribution from variable-length fields
	 *            such as owner/group.
	 */
	len = 16;

	/* Sigh */
	if (iap->ia_valid & ATTR_SIZE)
		len += 8;
	if (iap->ia_valid & ATTR_MODE)
		len += 4;
	if (iap->ia_valid & ATTR_UID) {
		owner_namelen = nfs_map_uid_to_name(server, iap->ia_uid, owner_name, IDMAP_NAMESZ);
		if (owner_namelen < 0) {
			dprintk("nfs: couldn't resolve uid %d to string\n",
					iap->ia_uid);
			/* XXX */
			strcpy(owner_name, "nobody");
			owner_namelen = sizeof("nobody") - 1;
			/* goto out; */
		}
		len += 4 + (XDR_QUADLEN(owner_namelen) << 2);
	}
	if (iap->ia_valid & ATTR_GID) {
		owner_grouplen = nfs_map_gid_to_group(server, iap->ia_gid, owner_group, IDMAP_NAMESZ);
		if (owner_grouplen < 0) {
			dprintk("nfs: couldn't resolve gid %d to string\n",
					iap->ia_gid);
			strcpy(owner_group, "nobody");
			owner_grouplen = sizeof("nobody") - 1;
			/* goto out; */
		}
		len += 4 + (XDR_QUADLEN(owner_grouplen) << 2);
	}
	if (iap->ia_valid & ATTR_ATIME_SET)
		len += 16;
	else if (iap->ia_valid & ATTR_ATIME)
		len += 4;
	if (iap->ia_valid & ATTR_MTIME_SET)
		len += 16;
	else if (iap->ia_valid & ATTR_MTIME)
		len += 4;
	p = reserve_space(xdr, len);

	/*
	 * We write the bitmap length now, but leave the bitmap and the attribute
	 * buffer length to be backfilled at the end of this routine.
	 */
	*p++ = cpu_to_be32(2);
	q = p;
	p += 3;

	if (iap->ia_valid & ATTR_SIZE) {
		bmval0 |= FATTR4_WORD0_SIZE;
		p = xdr_encode_hyper(p, iap->ia_size);
	}
	if (iap->ia_valid & ATTR_MODE) {
		bmval1 |= FATTR4_WORD1_MODE;
		*p++ = cpu_to_be32(iap->ia_mode & S_IALLUGO);
	}
	if (iap->ia_valid & ATTR_UID) {
		bmval1 |= FATTR4_WORD1_OWNER;
		p = xdr_encode_opaque(p, owner_name, owner_namelen);
	}
	if (iap->ia_valid & ATTR_GID) {
		bmval1 |= FATTR4_WORD1_OWNER_GROUP;
		p = xdr_encode_opaque(p, owner_group, owner_grouplen);
	}
	if (iap->ia_valid & ATTR_ATIME_SET) {
		bmval1 |= FATTR4_WORD1_TIME_ACCESS_SET;
		*p++ = cpu_to_be32(NFS4_SET_TO_CLIENT_TIME);
		*p++ = cpu_to_be32(0);
		*p++ = cpu_to_be32(iap->ia_atime.tv_sec);
		*p++ = cpu_to_be32(iap->ia_atime.tv_nsec);
	}
	else if (iap->ia_valid & ATTR_ATIME) {
		bmval1 |= FATTR4_WORD1_TIME_ACCESS_SET;
		*p++ = cpu_to_be32(NFS4_SET_TO_SERVER_TIME);
	}
	if (iap->ia_valid & ATTR_MTIME_SET) {
		bmval1 |= FATTR4_WORD1_TIME_MODIFY_SET;
		*p++ = cpu_to_be32(NFS4_SET_TO_CLIENT_TIME);
		*p++ = cpu_to_be32(0);
		*p++ = cpu_to_be32(iap->ia_mtime.tv_sec);
		*p++ = cpu_to_be32(iap->ia_mtime.tv_nsec);
	}
	else if (iap->ia_valid & ATTR_MTIME) {
		bmval1 |= FATTR4_WORD1_TIME_MODIFY_SET;
		*p++ = cpu_to_be32(NFS4_SET_TO_SERVER_TIME);
	}

	/*
	 * Now we backfill the bitmap and the attribute buffer length.
	 */
	if (len != ((char *)p - (char *)q) + 4) {
		printk(KERN_ERR "NFS: Attr length error, %u != %Zu\n",
				len, ((char *)p - (char *)q) + 4);
		BUG();
	}
	len = (char *)p - (char *)q - 12;
	*q++ = htonl(bmval0);
	*q++ = htonl(bmval1);
	*q = htonl(len);

/* out: */
}
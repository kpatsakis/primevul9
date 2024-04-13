static inline u32 nfsd4_getattr_rsize(struct svc_rqst *rqstp,
				      struct nfsd4_op *op)
{
	u32 *bmap = op->u.getattr.ga_bmval;
	u32 bmap0 = bmap[0], bmap1 = bmap[1], bmap2 = bmap[2];
	u32 ret = 0;

	if (bmap0 & FATTR4_WORD0_ACL)
		return svc_max_payload(rqstp);
	if (bmap0 & FATTR4_WORD0_FS_LOCATIONS)
		return svc_max_payload(rqstp);

	if (bmap1 & FATTR4_WORD1_OWNER) {
		ret += IDMAP_NAMESZ + 4;
		bmap1 &= ~FATTR4_WORD1_OWNER;
	}
	if (bmap1 & FATTR4_WORD1_OWNER_GROUP) {
		ret += IDMAP_NAMESZ + 4;
		bmap1 &= ~FATTR4_WORD1_OWNER_GROUP;
	}
	if (bmap0 & FATTR4_WORD0_FILEHANDLE) {
		ret += NFS4_FHSIZE + 4;
		bmap0 &= ~FATTR4_WORD0_FILEHANDLE;
	}
	if (bmap2 & FATTR4_WORD2_SECURITY_LABEL) {
		ret += NFS4_MAXLABELLEN + 12;
		bmap2 &= ~FATTR4_WORD2_SECURITY_LABEL;
	}
	/*
	 * Largest of remaining attributes are 16 bytes (e.g.,
	 * supported_attributes)
	 */
	ret += 16 * (hweight32(bmap0) + hweight32(bmap1) + hweight32(bmap2));
	/* bitmask, length */
	ret += 20;
	return ret;
}
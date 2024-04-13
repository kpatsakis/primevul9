_nfsd4_verify(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	     struct nfsd4_verify *verify)
{
	__be32 *buf, *p;
	int count;
	__be32 status;

	status = fh_verify(rqstp, &cstate->current_fh, 0, NFSD_MAY_NOP);
	if (status)
		return status;

	status = check_attr_support(rqstp, cstate, verify->ve_bmval, NULL);
	if (status)
		return status;

	if ((verify->ve_bmval[0] & FATTR4_WORD0_RDATTR_ERROR)
	    || (verify->ve_bmval[1] & NFSD_WRITEONLY_ATTRS_WORD1))
		return nfserr_inval;
	if (verify->ve_attrlen & 3)
		return nfserr_inval;

	/* count in words:
	 *   bitmap_len(1) + bitmap(2) + attr_len(1) = 4
	 */
	count = 4 + (verify->ve_attrlen >> 2);
	buf = kmalloc(count << 2, GFP_KERNEL);
	if (!buf)
		return nfserr_jukebox;

	p = buf;
	status = nfsd4_encode_fattr_to_buf(&p, count, &cstate->current_fh,
				    cstate->current_fh.fh_export,
				    cstate->current_fh.fh_dentry,
				    verify->ve_bmval,
				    rqstp, 0);
	/*
	 * If nfsd4_encode_fattr() ran out of space, assume that's because
	 * the attributes are longer (hence different) than those given:
	 */
	if (status == nfserr_resource)
		status = nfserr_not_same;
	if (status)
		goto out_kfree;

	/* skip bitmap */
	p = buf + 1 + ntohl(buf[0]);
	status = nfserr_not_same;
	if (ntohl(*p++) != verify->ve_attrlen)
		goto out_kfree;
	if (!memcmp(p, verify->ve_attrval, verify->ve_attrlen))
		status = nfserr_same;

out_kfree:
	kfree(buf);
	return status;
}
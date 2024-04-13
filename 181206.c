static __be32 nfsd4_do_lookupp(struct svc_rqst *rqstp, struct svc_fh *fh)
{
	struct svc_fh tmp_fh;
	__be32 ret;

	fh_init(&tmp_fh, NFS4_FHSIZE);
	ret = exp_pseudoroot(rqstp, &tmp_fh);
	if (ret)
		return ret;
	if (tmp_fh.fh_dentry == fh->fh_dentry) {
		fh_put(&tmp_fh);
		return nfserr_noent;
	}
	fh_put(&tmp_fh);
	return nfsd_lookup(rqstp, fh, "..", 2, fh);
}
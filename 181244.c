fh_dup2(struct svc_fh *dst, struct svc_fh *src)
{
	fh_put(dst);
	dget(src->fh_dentry);
	if (src->fh_export)
		exp_get(src->fh_export);
	*dst = *src;
}
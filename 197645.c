nfsd4_clone(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
		struct nfsd4_clone *clone)
{
	struct file *src, *dst;
	__be32 status;

	status = nfsd4_verify_copy(rqstp, cstate, &clone->cl_src_stateid, &src,
				   &clone->cl_dst_stateid, &dst);
	if (status)
		goto out;

	status = nfsd4_clone_file_range(src, clone->cl_src_pos,
			dst, clone->cl_dst_pos, clone->cl_count);

	fput(dst);
	fput(src);
out:
	return status;
}
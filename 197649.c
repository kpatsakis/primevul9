nfsd4_copy(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
		struct nfsd4_copy *copy)
{
	struct file *src, *dst;
	__be32 status;
	ssize_t bytes;

	status = nfsd4_verify_copy(rqstp, cstate, &copy->cp_src_stateid, &src,
				   &copy->cp_dst_stateid, &dst);
	if (status)
		goto out;

	bytes = nfsd_copy_file_range(src, copy->cp_src_pos,
			dst, copy->cp_dst_pos, copy->cp_count);

	if (bytes < 0)
		status = nfserrno(bytes);
	else {
		copy->cp_res.wr_bytes_written = bytes;
		copy->cp_res.wr_stable_how = NFS_UNSTABLE;
		copy->cp_consecutive = 1;
		copy->cp_synchronous = 1;
		gen_boot_verifier(&copy->cp_res.wr_verifier, SVC_NET(rqstp));
		status = nfs_ok;
	}

	fput(src);
	fput(dst);
out:
	return status;
}
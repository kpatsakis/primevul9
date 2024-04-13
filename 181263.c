static __be32 nfsd4_do_copy(struct nfsd4_copy *copy, bool sync)
{
	__be32 status;
	ssize_t bytes;

	bytes = _nfsd_copy_file_range(copy);
	/* for async copy, we ignore the error, client can always retry
	 * to get the error
	 */
	if (bytes < 0 && !copy->cp_res.wr_bytes_written)
		status = nfserrno(bytes);
	else {
		nfsd4_init_copy_res(copy, sync);
		status = nfs_ok;
	}

	fput(copy->file_src);
	fput(copy->file_dst);
	return status;
}
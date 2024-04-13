static __be32 nfsd_check_obj_isreg(struct svc_fh *fh)
{
	umode_t mode = d_inode(fh->fh_dentry)->i_mode;

	if (S_ISREG(mode))
		return nfs_ok;
	if (S_ISDIR(mode))
		return nfserr_isdir;
	/*
	 * Using err_symlink as our catch-all case may look odd; but
	 * there's no other obvious error for this case in 4.0, and we
	 * happen to know that it will cause the linux v4 client to do
	 * the right thing on attempts to open something other than a
	 * regular file.
	 */
	return nfserr_symlink;
}
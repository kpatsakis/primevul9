static int nfs4_xattr_set_nfs4_acl(struct dentry *dentry, const char *key,
				   const void *buf, size_t buflen,
				   int flags, int type)
{
	if (strcmp(key, "") != 0)
		return -EINVAL;

	return nfs4_proc_set_acl(dentry->d_inode, buf, buflen);
}
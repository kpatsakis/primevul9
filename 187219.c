static int nfs4_xattr_get_nfs4_acl(struct dentry *dentry, const char *key,
				   void *buf, size_t buflen, int type)
{
	if (strcmp(key, "") != 0)
		return -EINVAL;

	return nfs4_proc_get_acl(dentry->d_inode, buf, buflen);
}
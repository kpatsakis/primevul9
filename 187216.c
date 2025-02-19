static size_t nfs4_xattr_list_nfs4_acl(struct dentry *dentry, char *list,
				       size_t list_len, const char *name,
				       size_t name_len, int type)
{
	size_t len = sizeof(XATTR_NAME_NFSV4_ACL);

	if (!nfs4_server_supports_acls(NFS_SERVER(dentry->d_inode)))
		return 0;

	if (list && len <= list_len)
		memcpy(list, XATTR_NAME_NFSV4_ACL, len);
	return len;
}
static ssize_t nfs4_get_acl_uncached(struct inode *inode, void *buf, size_t buflen)
{
	struct nfs4_exception exception = { };
	ssize_t ret;
	do {
		ret = __nfs4_get_acl_uncached(inode, buf, buflen);
		if (ret >= 0)
			break;
		ret = nfs4_handle_exception(NFS_SERVER(inode), ret, &exception);
	} while (exception.retry);
	return ret;
}
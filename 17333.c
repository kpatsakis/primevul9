static int filename_lookup(int dfd, struct filename *name,
				unsigned int flags, struct nameidata *nd)
{
	int retval = path_lookupat(dfd, name->name, flags | LOOKUP_RCU, nd);
	if (unlikely(retval == -ECHILD))
		retval = path_lookupat(dfd, name->name, flags, nd);
	if (unlikely(retval == -ESTALE))
		retval = path_lookupat(dfd, name->name,
						flags | LOOKUP_REVAL, nd);

	if (likely(!retval))
		audit_inode(name, nd->path.dentry, flags & LOOKUP_PARENT);
	return retval;
}
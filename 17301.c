static struct dentry *lookup_hash(struct nameidata *nd)
{
	return __lookup_hash(&nd->last, nd->path.dentry, nd->flags);
}
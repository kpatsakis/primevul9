static struct dentry *lookup_dcache(struct qstr *name, struct dentry *dir,
				    unsigned int flags, bool *need_lookup)
{
	struct dentry *dentry;
	int error;

	*need_lookup = false;
	dentry = d_lookup(dir, name);
	if (dentry) {
		if (dentry->d_flags & DCACHE_OP_REVALIDATE) {
			error = d_revalidate(dentry, flags);
			if (unlikely(error <= 0)) {
				if (error < 0) {
					dput(dentry);
					return ERR_PTR(error);
				} else if (!d_invalidate(dentry)) {
					dput(dentry);
					dentry = NULL;
				}
			}
		}
	}

	if (!dentry) {
		dentry = d_alloc(dir, name);
		if (unlikely(!dentry))
			return ERR_PTR(-ENOMEM);

		*need_lookup = true;
	}
	return dentry;
}
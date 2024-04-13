user_path_parent(int dfd, const char __user *path, struct nameidata *nd,
		 unsigned int flags)
{
	struct filename *s = getname(path);
	int error;

	/* only LOOKUP_REVAL is allowed in extra flags */
	flags &= LOOKUP_REVAL;

	if (IS_ERR(s))
		return s;

	error = filename_lookup(dfd, s, flags | LOOKUP_PARENT, nd);
	if (error) {
		putname(s);
		return ERR_PTR(error);
	}

	return s;
}
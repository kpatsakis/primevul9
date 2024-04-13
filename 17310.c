int user_path_at_empty(int dfd, const char __user *name, unsigned flags,
		 struct path *path, int *empty)
{
	struct nameidata nd;
	struct filename *tmp = getname_flags(name, flags, empty);
	int err = PTR_ERR(tmp);
	if (!IS_ERR(tmp)) {

		BUG_ON(flags & LOOKUP_PARENT);

		err = filename_lookup(dfd, tmp, flags, &nd);
		putname(tmp);
		if (!err)
			*path = nd.path;
	}
	return err;
}
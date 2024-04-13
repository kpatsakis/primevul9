getname_flags(const char __user *filename, int flags, int *empty)
{
	struct filename *result, *err;
	int len;
	long max;
	char *kname;

	result = audit_reusename(filename);
	if (result)
		return result;

	result = __getname();
	if (unlikely(!result))
		return ERR_PTR(-ENOMEM);

	/*
	 * First, try to embed the struct filename inside the names_cache
	 * allocation
	 */
	kname = (char *)result + sizeof(*result);
	result->name = kname;
	result->separate = false;
	max = EMBEDDED_NAME_MAX;

recopy:
	len = strncpy_from_user(kname, filename, max);
	if (unlikely(len < 0)) {
		err = ERR_PTR(len);
		goto error;
	}

	/*
	 * Uh-oh. We have a name that's approaching PATH_MAX. Allocate a
	 * separate struct filename so we can dedicate the entire
	 * names_cache allocation for the pathname, and re-do the copy from
	 * userland.
	 */
	if (len == EMBEDDED_NAME_MAX && max == EMBEDDED_NAME_MAX) {
		kname = (char *)result;

		result = kzalloc(sizeof(*result), GFP_KERNEL);
		if (!result) {
			err = ERR_PTR(-ENOMEM);
			result = (struct filename *)kname;
			goto error;
		}
		result->name = kname;
		result->separate = true;
		max = PATH_MAX;
		goto recopy;
	}

	/* The empty path is special. */
	if (unlikely(!len)) {
		if (empty)
			*empty = 1;
		err = ERR_PTR(-ENOENT);
		if (!(flags & LOOKUP_EMPTY))
			goto error;
	}

	err = ERR_PTR(-ENAMETOOLONG);
	if (unlikely(len >= PATH_MAX))
		goto error;

	result->uptr = filename;
	audit_getname(result);
	return result;

error:
	final_putname(result);
	return err;
}
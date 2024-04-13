dir_each_entry(VALUE dir, VALUE (*each)(VALUE, VALUE), VALUE arg, int children_only)
{
    struct dir_data *dirp;
    struct dirent *dp;
    IF_NORMALIZE_UTF8PATH(int norm_p);

    GetDIR(dir, dirp);
    rewinddir(dirp->dir);
    IF_NORMALIZE_UTF8PATH(norm_p = need_normalization(dirp->dir, RSTRING_PTR(dirp->path)));
    while ((dp = READDIR(dirp->dir, dirp->enc)) != NULL) {
	const char *name = dp->d_name;
	size_t namlen = NAMLEN(dp);
	VALUE path;

	if (children_only && name[0] == '.') {
	    if (namlen == 1) continue; /* current directory */
	    if (namlen == 2 && name[1] == '.') continue; /* parent directory */
	}
#if NORMALIZE_UTF8PATH
	if (norm_p && has_nonascii(name, namlen) &&
	    !NIL_P(path = rb_str_normalize_ospath(name, namlen))) {
	    path = rb_external_str_with_enc(path, dirp->enc);
	}
	else
#endif
	path = rb_external_str_new_with_enc(name, namlen, dirp->enc);
	(*each)(arg, path);
    }
    return dir;
}
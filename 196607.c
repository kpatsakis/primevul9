do_opendir(const int basefd, size_t baselen, const char *path, int flags, rb_encoding *enc,
	   ruby_glob_errfunc *errfunc, VALUE arg, int *status)
{
    DIR *dirp;
#ifdef _WIN32
    VALUE tmp = 0;
    if (!fundamental_encoding_p(enc)) {
	tmp = rb_enc_str_new(path, strlen(path), enc);
	tmp = rb_str_encode_ospath(tmp);
	path = RSTRING_PTR(tmp);
    }
#endif
    dirp = opendir_at(basefd, at_subpath(basefd, baselen, path));
    if (!dirp) {
	int e = errno;

	*status = 0;
	if (!to_be_ignored(e)) {
	    if (errfunc) {
		*status = (*errfunc)(path, arg, enc, e);
	    }
	    else {
		sys_warning(path, enc);
	    }
	}
    }
#ifdef _WIN32
    if (tmp) rb_str_resize(tmp, 0); /* GC guard */
#endif

    return dirp;
}
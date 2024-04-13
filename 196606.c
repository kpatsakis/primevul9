push_glob(VALUE ary, VALUE str, VALUE base, int flags)
{
    struct glob_args args;
    int fd;
    rb_encoding *enc = rb_enc_get(str);

#if defined _WIN32 || defined __APPLE__
    str = rb_str_encode_ospath(str);
#endif
    if (rb_enc_to_index(enc) == ENCINDEX_US_ASCII)
	enc = rb_filesystem_encoding();
    if (rb_enc_to_index(enc) == ENCINDEX_US_ASCII)
	enc = rb_ascii8bit_encoding();
    flags |= GLOB_VERBOSE;
    args.func = push_pattern;
    args.value = ary;
    args.enc = enc;
    args.base = 0;
    fd = AT_FDCWD;
    if (!NIL_P(base)) {
	if (!RB_TYPE_P(base, T_STRING) || !rb_enc_check(str, base)) {
	    struct dir_data *dirp = DATA_PTR(base);
	    if (!dirp->dir) dir_closed();
#ifdef HAVE_DIRFD
	    if ((fd = dirfd(dirp->dir)) == -1)
		rb_sys_fail_path(dir_inspect(base));
#endif
	    base = dirp->path;
	}
	args.base = RSTRING_PTR(base);
    }
#if defined _WIN32 || defined __APPLE__
    enc = rb_utf8_encoding();
#endif

    return ruby_glob0(RSTRING_PTR(str), fd, args.base, flags, &rb_glob_funcs,
		      (VALUE)&args, enc);
}
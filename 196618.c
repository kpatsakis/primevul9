ruby_glob0(const char *path, int fd, const char *base, int flags,
	   const ruby_glob_funcs_t *funcs, VALUE arg,
	   rb_encoding *enc)
{
    struct glob_pattern *list;
    const char *root, *start;
    char *buf;
    size_t n, baselen = 0;
    int status, dirsep = FALSE;

    start = root = path;

    if (*root == '{') {
        struct push_glob0_args args;
        args.fd = fd;
        args.base = base;
        args.flags = flags;
        args.funcs = funcs;
        args.arg = arg;
        return ruby_brace_expand(path, flags, push_glob0_caller, (VALUE)&args, enc, Qfalse);
    }

    flags |= FNM_SYSCASE;
#if defined DOSISH
    root = rb_enc_path_skip_prefix(root, root + strlen(root), enc);
#endif

    if (*root == '/') root++;

    n = root - start;
    if (!n && base) {
	n = strlen(base);
	baselen = n;
	start = base;
	dirsep = TRUE;
    }
    buf = GLOB_ALLOC_N(char, n + 1);
    if (!buf) return -1;
    MEMCPY(buf, start, char, n);
    buf[n] = '\0';

    list = glob_make_pattern(root, root + strlen(root), flags, enc);
    if (!list) {
	GLOB_FREE(buf);
	return -1;
    }
    status = glob_helper(fd, buf, baselen, n-baselen, dirsep,
			 path_unknown, &list, &list + 1,
			 flags, funcs, arg, enc);
    glob_free_pattern(list);
    GLOB_FREE(buf);

    return status;
}
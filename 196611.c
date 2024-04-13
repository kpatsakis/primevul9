glob_helper(
    int fd,
    const char *path,
    size_t baselen,
    size_t namelen,
    int dirsep, /* '/' should be placed before appending child entry's name to 'path'. */
    rb_pathtype_t pathtype, /* type of 'path' */
    struct glob_pattern **beg,
    struct glob_pattern **end,
    int flags,
    const ruby_glob_funcs_t *funcs,
    VALUE arg,
    rb_encoding *enc)
{
    struct stat st;
    int status = 0;
    struct glob_pattern **cur, **new_beg, **new_end;
    int plain = 0, brace = 0, magical = 0, recursive = 0, match_all = 0, match_dir = 0;
    int escape = !(flags & FNM_NOESCAPE);
    size_t pathlen = baselen + namelen;

    for (cur = beg; cur < end; ++cur) {
	struct glob_pattern *p = *cur;
	if (p->type == RECURSIVE) {
	    recursive = 1;
	    p = p->next;
	}
	switch (p->type) {
	  case PLAIN:
	    plain = 1;
	    break;
	  case ALPHA:
#if USE_NAME_ON_FS == USE_NAME_ON_FS_REAL_BASENAME
	    plain = 1;
#else
	    magical = 1;
#endif
	    break;
	  case BRACE:
	    if (!recursive) {
		brace = 1;
	    }
	    break;
	  case MAGICAL:
	    magical = 2;
	    break;
	  case MATCH_ALL:
	    match_all = 1;
	    break;
	  case MATCH_DIR:
	    match_dir = 1;
	    break;
	  case RECURSIVE:
	    rb_bug("continuous RECURSIVEs");
	}
    }

    if (brace) {
	struct push_glob_args args;
	char* brace_path = join_path_from_pattern(beg);
	if (!brace_path) return -1;
	args.fd = fd;
	args.path = path;
	args.baselen = baselen;
	args.namelen = namelen;
	args.dirsep = dirsep;
	args.pathtype = pathtype;
	args.flags = flags;
	args.funcs = funcs;
	args.arg = arg;
	status = ruby_brace_expand(brace_path, flags, push_caller, (VALUE)&args, enc, Qfalse);
	GLOB_FREE(brace_path);
	return status;
    }

    if (*path) {
	if (match_all && pathtype == path_unknown) {
	    if (do_lstat(fd, baselen, path, &st, flags, enc) == 0) {
		pathtype = IFTODT(st.st_mode);
	    }
	    else {
		pathtype = path_noent;
	    }
	}
	if (match_dir && (pathtype == path_unknown || pathtype == path_symlink)) {
	    if (do_stat(fd, baselen, path, &st, flags, enc) == 0) {
		pathtype = IFTODT(st.st_mode);
	    }
	    else {
		pathtype = path_noent;
	    }
	}
	if (match_all && pathtype > path_noent) {
	    const char *subpath = path + baselen + (baselen && path[baselen] == '/');
	    status = glob_call_func(funcs->match, subpath, arg, enc);
	    if (status) return status;
	}
	if (match_dir && pathtype == path_directory) {
	    int seplen = (baselen && path[baselen] == '/');
	    const char *subpath = path + baselen + seplen;
	    char *tmp = join_path(subpath, namelen - seplen, dirsep, "", 0);
	    if (!tmp) return -1;
	    status = glob_call_func(funcs->match, tmp, arg, enc);
	    GLOB_FREE(tmp);
	    if (status) return status;
	}
    }

    if (pathtype == path_noent) return 0;

    if (magical || recursive) {
	struct dirent *dp;
	DIR *dirp;
# if USE_NAME_ON_FS == USE_NAME_ON_FS_BY_FNMATCH
	char *plainname = 0;
# endif
	IF_NORMALIZE_UTF8PATH(int norm_p);
# if USE_NAME_ON_FS == USE_NAME_ON_FS_BY_FNMATCH
	if (cur + 1 == end && (*cur)->type <= ALPHA) {
	    plainname = join_path(path, pathlen, dirsep, (*cur)->str, strlen((*cur)->str));
	    if (!plainname) return -1;
	    dirp = do_opendir(fd, basename, plainname, flags, enc, funcs->error, arg, &status);
	    GLOB_FREE(plainname);
	}
	else
# else
	    ;
# endif
	dirp = do_opendir(fd, baselen, path, flags, enc, funcs->error, arg, &status);
	if (dirp == NULL) {
# if FNM_SYSCASE || NORMALIZE_UTF8PATH
	    if ((magical < 2) && !recursive && (errno == EACCES)) {
		/* no read permission, fallback */
		goto literally;
	    }
# endif
	    return status;
	}
	IF_NORMALIZE_UTF8PATH(norm_p = need_normalization(dirp, *path ? path : "."));

# if NORMALIZE_UTF8PATH
	if (!(norm_p || magical || recursive)) {
	    closedir(dirp);
	    goto literally;
	}
# endif
# ifdef HAVE_GETATTRLIST
	if (is_case_sensitive(dirp, path) == 0)
	    flags |= FNM_CASEFOLD;
# endif
	while ((dp = READDIR(dirp, enc)) != NULL) {
	    char *buf;
	    rb_pathtype_t new_pathtype = path_unknown;
	    const char *name;
	    size_t namlen;
	    int dotfile = 0;
	    IF_NORMALIZE_UTF8PATH(VALUE utf8str = Qnil);

	    name = dp->d_name;
	    namlen = NAMLEN(dp);
	    if (recursive && name[0] == '.') {
		++dotfile;
		if (namlen == 1) {
		    /* unless DOTMATCH, skip current directories not to recurse infinitely */
		    if (!(flags & FNM_DOTMATCH)) continue;
		    ++dotfile;
		    new_pathtype = path_directory; /* force to skip stat/lstat */
		}
		else if (namlen == 2 && name[1] == '.') {
		    /* always skip parent directories not to recurse infinitely */
		    continue;
		}
	    }

# if NORMALIZE_UTF8PATH
	    if (norm_p && has_nonascii(name, namlen)) {
		if (!NIL_P(utf8str = rb_str_normalize_ospath(name, namlen))) {
		    RSTRING_GETMEM(utf8str, name, namlen);
		}
	    }
# endif
	    buf = join_path(path, pathlen, dirsep, name, namlen);
	    IF_NORMALIZE_UTF8PATH(if (!NIL_P(utf8str)) rb_str_resize(utf8str, 0));
	    if (!buf) {
		status = -1;
		break;
	    }
	    name = buf + pathlen + (dirsep != 0);
#ifdef DT_UNKNOWN
	    if (dp->d_type != DT_UNKNOWN) {
		/* Got it. We need no more lstat. */
		new_pathtype = dp->d_type;
	    }
#endif
	    if (recursive && dotfile < ((flags & FNM_DOTMATCH) ? 2 : 1) &&
		new_pathtype == path_unknown) {
		/* RECURSIVE never match dot files unless FNM_DOTMATCH is set */
		if (do_lstat(fd, baselen, buf, &st, flags, enc) == 0)
		    new_pathtype = IFTODT(st.st_mode);
		else
		    new_pathtype = path_noent;
	    }

	    new_beg = new_end = GLOB_ALLOC_N(struct glob_pattern *, (end - beg) * 2);
	    if (!new_beg) {
		GLOB_FREE(buf);
		status = -1;
		break;
	    }

	    for (cur = beg; cur < end; ++cur) {
		struct glob_pattern *p = *cur;
		struct dirent_brace_args args;
		if (p->type == RECURSIVE) {
		    if (new_pathtype == path_directory || /* not symlink but real directory */
			new_pathtype == path_exist) {
			if (dotfile < ((flags & FNM_DOTMATCH) ? 2 : 1))
			    *new_end++ = p; /* append recursive pattern */
		    }
		    p = p->next; /* 0 times recursion */
		}
		switch (p->type) {
		  case BRACE:
		    args.name = name;
		    args.dp = dp;
		    args.flags = flags;
		    if (ruby_brace_expand(p->str, flags, dirent_match_brace,
					  (VALUE)&args, enc, Qfalse) > 0)
			*new_end++ = p->next;
		    break;
		  case ALPHA:
# if USE_NAME_ON_FS == USE_NAME_ON_FS_BY_FNMATCH
		    if (plainname) {
			*new_end++ = p->next;
			break;
		    }
# endif
		  case PLAIN:
		  case MAGICAL:
		    if (dirent_match(p->str, enc, name, dp, flags))
			*new_end++ = p->next;
		  default:
		    break;
		}
	    }

	    status = glob_helper(fd, buf, baselen, name - buf - baselen + namlen, 1,
				 new_pathtype, new_beg, new_end,
				 flags, funcs, arg, enc);
	    GLOB_FREE(buf);
	    GLOB_FREE(new_beg);
	    if (status) break;
	}

	closedir(dirp);
    }
    else if (plain) {
	struct glob_pattern **copy_beg, **copy_end, **cur2;

# if FNM_SYSCASE || NORMALIZE_UTF8PATH
      literally:
# endif
	copy_beg = copy_end = GLOB_ALLOC_N(struct glob_pattern *, end - beg);
	if (!copy_beg) return -1;
	for (cur = beg; cur < end; ++cur)
	    *copy_end++ = (*cur)->type <= ALPHA ? *cur : 0;

	for (cur = copy_beg; cur < copy_end; ++cur) {
	    if (*cur) {
		rb_pathtype_t new_pathtype = path_unknown;
		char *buf;
		char *name;
		size_t len = strlen((*cur)->str) + 1;
		name = GLOB_ALLOC_N(char, len);
		if (!name) {
		    status = -1;
		    break;
		}
		memcpy(name, (*cur)->str, len);
		if (escape)
		    len = remove_backslashes(name, name+len-1, enc) - name;

		new_beg = new_end = GLOB_ALLOC_N(struct glob_pattern *, end - beg);
		if (!new_beg) {
		    GLOB_FREE(name);
		    status = -1;
		    break;
		}
		*new_end++ = (*cur)->next;
		for (cur2 = cur + 1; cur2 < copy_end; ++cur2) {
		    if (*cur2 && fnmatch((*cur2)->str, enc, name, flags) == 0) {
			*new_end++ = (*cur2)->next;
			*cur2 = 0;
		    }
		}

		buf = join_path(path, pathlen, dirsep, name, len);
		GLOB_FREE(name);
		if (!buf) {
		    GLOB_FREE(new_beg);
		    status = -1;
		    break;
		}
#if USE_NAME_ON_FS == USE_NAME_ON_FS_REAL_BASENAME
		if ((*cur)->type == ALPHA) {
		    buf = replace_real_basename(buf, pathlen + (dirsep != 0), enc,
						IF_NORMALIZE_UTF8PATH(1)+0,
						flags, &new_pathtype);
		    if (!buf) break;
		}
#endif
		status = glob_helper(fd, buf, baselen,
				     namelen + strlen(buf + pathlen), 1,
				     new_pathtype, new_beg, new_end,
				     flags, funcs, arg, enc);
		GLOB_FREE(buf);
		GLOB_FREE(new_beg);
		if (status) break;
	    }
	}

	GLOB_FREE(copy_beg);
    }

    return status;
}
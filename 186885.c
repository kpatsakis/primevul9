file_fsmagic(struct magic_set *ms, const char *fn, struct stat *sb, php_stream *stream)
{
	int ret, did = 0;
	int mime = ms->flags & MAGIC_MIME;
	TSRMLS_FETCH();

	if (ms->flags & MAGIC_APPLE)
		return 0;

	if (fn == NULL && !stream) {
		return 0;
	}

#define COMMA	(did++ ? ", " : "")

	if (stream) {
		php_stream_statbuf ssb;
		if (php_stream_stat(stream, &ssb) < 0) {
			if (ms->flags & MAGIC_ERROR) {
				file_error(ms, errno, "cannot stat `%s'", fn);
				return -1;
			}
			return 1;
		}
		memcpy(sb, &ssb.sb, sizeof(struct stat));
	} else {
		if (php_sys_stat(fn, sb) != 0) {
			if (ms->flags & MAGIC_ERROR) {
				file_error(ms, errno, "cannot stat `%s'", fn);
				return -1;
			}
			return 1;
		}
	}

	ret = 1;
	if (!mime) {
#ifdef S_ISUID
		if (sb->st_mode & S_ISUID) 
			if (file_printf(ms, "%ssetuid", COMMA) == -1)
				return -1;
#endif
#ifdef S_ISGID
		if (sb->st_mode & S_ISGID) 
			if (file_printf(ms, "%ssetgid", COMMA) == -1)
				return -1;
#endif
#ifdef S_ISVTX
		if (sb->st_mode & S_ISVTX) 
			if (file_printf(ms, "%ssticky", COMMA) == -1)
				return -1;
#endif
	}
	
	switch (sb->st_mode & S_IFMT) {
#ifndef PHP_WIN32
# ifdef S_IFCHR
		case S_IFCHR:
			/* 
			 * If -s has been specified, treat character special files
			 * like ordinary files.  Otherwise, just report that they
			 * are block special files and go on to the next file.
			 */
			if ((ms->flags & MAGIC_DEVICES) != 0) {
				ret = 0;
				break;
			}
			if (mime) {
				if (handle_mime(ms, mime, "x-character-device") == -1)
					return -1;
			} else {
#  ifdef HAVE_STAT_ST_RDEV
#   ifdef dv_unit
			if (file_printf(ms, "%scharacter special (%d/%d/%d)",
			    COMMA, major(sb->st_rdev), dv_unit(sb->st_rdev),
						dv_subunit(sb->st_rdev)) == -1)
					return -1;
#   else
			if (file_printf(ms, "%scharacter special (%ld/%ld)",
			    COMMA, (long)major(sb->st_rdev),
			    (long)minor(sb->st_rdev)) == -1)
					return -1;
#   endif
#  else
			if (file_printf(ms, "%scharacter special", COMMA) == -1)
					return -1;
#  endif
			}
			return 1;
# endif
#endif

#ifdef	S_IFIFO
	case S_IFIFO:
		if((ms->flags & MAGIC_DEVICES) != 0)
			break;
		if (mime) {
			if (handle_mime(ms, mime, "fifo") == -1)
				return -1;
		} else if (file_printf(ms, "%sfifo (named pipe)", COMMA) == -1)
			return -1;
		break;
#endif
#ifdef	S_IFDOOR
	case S_IFDOOR:
		if (mime) {
			if (handle_mime(ms, mime, "door") == -1)
				return -1;
		} else if (file_printf(ms, "%sdoor", COMMA) == -1)
			return -1;
		break;
#endif
#ifdef	S_IFLNK
	case S_IFLNK:
		/* stat is used, if it made here then the link is broken */
			if (ms->flags & MAGIC_ERROR) {
			    file_error(ms, errno, "unreadable symlink `%s'", fn);
			    return -1;
			}
	return 1;
#endif

#ifdef	S_IFSOCK
#ifndef __COHERENT__
	case S_IFSOCK:
		if (mime) {
			if (handle_mime(ms, mime, "socket") == -1)
				return -1;
		} else if (file_printf(ms, "%ssocket", COMMA) == -1)
			return -1;
		break;
#endif
#endif
		case S_IFREG:
	/*
	 * regular file, check next possibility
	 *
	 * If stat() tells us the file has zero length, report here that
	 * the file is empty, so we can skip all the work of opening and 
	 * reading the file.
		 * But if the -s option has been given, we skip this
		 * optimization, since on some systems, stat() reports zero
		 * size for raw disk partitions. (If the block special device
		 * really has zero length, the fact that it is empty will be
		 * detected and reported correctly when we read the file.)
	 */
	if ((ms->flags & MAGIC_DEVICES) == 0 && sb->st_size == 0) {
		if (mime) {
			if (handle_mime(ms, mime, "x-empty") == -1)
				return -1;
			} else if (file_printf(ms, "%sempty", COMMA) == -1)
			return -1;
			break;
	}
		ret = 0;
		break;

	default:
		file_error(ms, 0, "invalid mode 0%o", sb->st_mode);
		return -1;
		/*NOTREACHED*/
	}

	return ret;
}
static int php_zip_ops_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC) /* {{{ */
{
	struct zip_stat sb;
	const char *path = stream->orig_path;
	int path_len = strlen(stream->orig_path);
	char *file_basename;
	size_t file_basename_len;
	char file_dirname[MAXPATHLEN];
	struct zip *za;
	char *fragment;
	int fragment_len;
	int err;

	fragment = strchr(path, '#');
	if (!fragment) {
		return -1;
	}


	if (strncasecmp("zip://", path, 6) == 0) {
		path += 6;
	}

	fragment_len = strlen(fragment);

	if (fragment_len < 1) {
		return -1;
	}
	path_len = strlen(path);
	if (path_len >= MAXPATHLEN) {
		return -1;
	}

	memcpy(file_dirname, path, path_len - fragment_len);
	file_dirname[path_len - fragment_len] = '\0';

	php_basename((char *)path, path_len - fragment_len, NULL, 0, &file_basename, &file_basename_len TSRMLS_CC);
	fragment++;

	if (ZIP_OPENBASEDIR_CHECKPATH(file_dirname)) {
		efree(file_basename);
		return -1;
	}

	za = zip_open(file_dirname, ZIP_CREATE, &err);
	if (za) {
		memset(ssb, 0, sizeof(php_stream_statbuf));
		if (zip_stat(za, fragment, ZIP_FL_NOCASE, &sb) != 0) {
			efree(file_basename);
			return -1;
		}
		zip_close(za);

		if (path[path_len-1] != '/') {
			ssb->sb.st_size = sb.size;
			ssb->sb.st_mode |= S_IFREG; /* regular file */
		} else {
			ssb->sb.st_size = 0;
			ssb->sb.st_mode |= S_IFDIR; /* regular directory */
		}

		ssb->sb.st_mtime = sb.mtime;
		ssb->sb.st_atime = sb.mtime;
		ssb->sb.st_ctime = sb.mtime;
		ssb->sb.st_nlink = 1;
		ssb->sb.st_rdev = -1;
#ifndef PHP_WIN32
		ssb->sb.st_blksize = -1;
		ssb->sb.st_blocks = -1;
#endif
		ssb->sb.st_ino = -1;
	}
	efree(file_basename);
	return 0;
}
static size_t php_plain_files_dirstream_read(php_stream *stream, char *buf, size_t count)
{
	DIR *dir = (DIR*)stream->abstract;
	/* avoid libc5 readdir problems */
	char entry[sizeof(struct dirent)+MAXPATHLEN];
	struct dirent *result = (struct dirent *)&entry;
	php_stream_dirent *ent = (php_stream_dirent*)buf;

	/* avoid problems if someone mis-uses the stream */
	if (count != sizeof(php_stream_dirent))
		return 0;

	if (php_readdir_r(dir, (struct dirent *)entry, &result) == 0 && result) {
		PHP_STRLCPY(ent->d_name, result->d_name, sizeof(ent->d_name), strlen(result->d_name));
		return sizeof(php_stream_dirent);
	}
	return 0;
}
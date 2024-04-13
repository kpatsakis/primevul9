static int local_file_url_prefixlen(const char *file_url)
{
	int len = -1;

	if (git__prefixcmp(file_url, "file://") == 0) {
		if (file_url[7] == '/')
			len = 8;
		else if (git__prefixcmp(file_url + 7, "localhost/") == 0)
			len = 17;
	}

	return len;
}
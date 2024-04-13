static int check_link_count (const char *file)
{
	struct stat sb;

	if (stat (file, &sb) != 0) {
		return 0;
	}

	if (sb.st_nlink != 2) {
		return 0;
	}

	return 1;
}
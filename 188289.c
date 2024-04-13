int git_path_cmp(
	const char *name1, size_t len1, int isdir1,
	const char *name2, size_t len2, int isdir2,
	int (*compare)(const char *, const char *, size_t))
{
	unsigned char c1, c2;
	size_t len = len1 < len2 ? len1 : len2;
	int cmp;

	cmp = compare(name1, name2, len);
	if (cmp)
		return cmp;

	c1 = name1[len];
	c2 = name2[len];

	if (c1 == '\0' && isdir1)
		c1 = '/';

	if (c2 == '\0' && isdir2)
		c2 = '/';

	return (c1 < c2) ? -1 : (c1 > c2) ? 1 : 0;
}
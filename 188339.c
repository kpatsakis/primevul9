GIT_INLINE(size_t) common_prefix_icase(const char *str, size_t len, const char *prefix)
{
	size_t count = 0;

	while (len >0 && tolower(*str) == tolower(*prefix)) {
		count++;
		str++;
		prefix++;
		len--;
	}

	return count;
}